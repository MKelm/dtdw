#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <ctype.h>
#include "display.h"

int maxy, maxx;
WINDOW *header, *footer, *output, *input;

void dsp_init(void) {
  setlocale(LC_ALL, "");
  initscr();
  getmaxyx(stdscr, maxy, maxx);
  start_color();
  init_pair(1, COLOR_WHITE, COLOR_BLUE); // for header and footer window
  init_pair(2, COLOR_WHITE, COLOR_BLACK); // for output window
  init_pair(3, COLOR_GREEN, COLOR_BLACK); // for output window items
  init_pair(4, COLOR_CYAN, COLOR_BLACK); // for output window transitions
  init_pair(5, COLOR_YELLOW, COLOR_BLACK); // for input window
  curs_set(0);
  keypad(stdscr, TRUE);
}

void dsp_end(void) {
  endwin();
}

void dsp_windows_init(void) {
  header = newwin(1, maxx, 0, 0);
  footer = newwin(1, maxx, maxy-1, 0);
  output = newwin(maxy-5, maxx-2, 2, 1);
  input = newwin(1, maxx, maxy-2, 0);
  refresh();

  wbkgd(header, COLOR_PAIR(1));
  wrefresh(header);

  wbkgd(footer, COLOR_PAIR(1));
  wrefresh(footer);

  wbkgd(output, COLOR_PAIR(2));
  wattrset(output, A_BOLD);
  scrollok(output, TRUE);
  wrefresh(output);

  wbkgd(input, COLOR_PAIR(5));
  wattrset(input, A_BOLD);
  wrefresh(input);
}

void dsp_set_meta(struct meta *meta_data) {
  char chstr[256];

  snprintf(chstr, 256, "%s, %s", meta_data->title, meta_data->version);
  mvwaddstr(footer, 0, 1, chstr);
  wrefresh(footer);

  snprintf(chstr, 256, "%s, %d", meta_data->author, meta_data->cyear);
  mvwaddstr(footer, 0, maxx-strlen(chstr)-1, chstr);
  wrefresh(footer);
}

void dsp_set_location(struct area *area_data, struct place *place_data) {
  wclear(header);
  char chstr[256];
  snprintf(chstr, 256, "%s, %s", area_data->title, place_data->title);
  mvwaddstr(header, 0, 1, chstr);
  wrefresh(header);
}

void dsp_set_output(char *str) {
  char buffer[2048];
  waddstr(output, dsp_word_wrap(buffer, str, maxx-2));
  wrefresh(output);
}

char *dsp_word_wrap(char* buffer, char* string, int line_width) {
  int i = 0;
  int k, counter;

  while (i < strlen(string)) {
    // copy string until the end of the line is reached
    for (counter = 1; counter <= line_width; counter++) {
      // check if end of string reached
      if (i == strlen(string)) {
        buffer[i] = 0;
        return buffer;
      }
      buffer[i] = string[i];
      // check for newlines embedded in the original input
      // and reset the index
      if (buffer[i] == '\n') {
        counter = 1;
      }
      i++;
    }
    // check for whitespace
    if (isspace(string[i])) {
      buffer[i] = '\n';
      i++;
    } else {
      // check for nearest whitespace back in string
      for (k = i; k > 0; k--) {
        if (isspace(string[k])) {
          buffer[k] = '\n';
          // set string index back to character after this one
          i = k + 1;
          break;
        }
      }
    }
  }
  buffer[i] = 0;

  return buffer;
}

char *dsp_get_input(void) {
  static char str[128];
  wclear(input);
  mvwaddstr(input, 0, 1, ">> ");
  curs_set(1);
  mvwgetnstr(input, 0, 4, str, 127);
  curs_set(0);
  return str;
}
