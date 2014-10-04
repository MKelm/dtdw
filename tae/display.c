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
  char chstr[256];

  snprintf(chstr, 256, "%s, %s", area_data->title, place_data->title);
  mvwaddstr(header, 0, 1, chstr);
  wrefresh(header);
}

void dsp_set_output(char *str) {
  // output for debugging
  wmove(output, 0, 0);
  waddstr(output, str);
  wrefresh(output);
}

void dsp_set_output_old(char *str) {
  char *t;
  t = str;
  int line = 1, pos = 0, tmppos = 0, attr = 0, len = strlen(str), llen = maxx-3;
  curs_set(1);

  wmove(output, line, 1);
  while (pos < len) {
    // mark items and transitions
    if (*t == '$' || *t == '#') {
      if (attr == 0) {
        wattron(output, COLOR_PAIR((*t == '$') ? 3 : 4) | A_BOLD);
        attr = 1;
      } else {
        wattroff(output, COLOR_PAIR((*t == '$') ? 3 : 4) | A_BOLD);
        attr = 0;
      }
      len--;
      t++;
    }

    if (pos > 0 && pos % llen == 0) {
      // word wrap, move word to next line on word break
      if (!isspace(*(t)) && !isspace(*(t+1))) {
        tmppos = pos;
        while (!isspace(*(t))) {
          // sometimes mvwdelch does not work properly, to inspect later
          mvwdelch(output, line, tmppos);
          tmppos--;
          t--;
          len++;
        };
      }
    } else if (pos > 1 && pos % llen == 1) {
      // got to next line if the new line has been reached
      line++;
      if (isspace(*(t))) {
        t++;
        pos++;
      }
      wmove(output, line, 1);
    }

    char ch[2];
    snprintf(ch, 2, "%s", t++); // workaround for unicode chars
    waddstr(output, ch);
    pos++;
  }
  wrefresh(output);
}

char *dsp_get_input(void) {
  static char str[25];
  wclear(input);
  mvwaddstr(input, 0, 1, ">> ");
  curs_set(1);
  mvwgetnstr(input, 0, 4, str, 24);
  curs_set(0);
  return str;
}
