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

  output = newwin(maxy-2, maxx, 1, 0);
  input = newwin(1, maxx, maxy-2, 0);
  refresh();

  wbkgd(header, COLOR_PAIR(1));
  mvwaddstr(header, 0, 1, "Area one, place two");
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
  mvwaddstr(header, 0, maxx-strlen(chstr), chstr);
  wrefresh(header);

  snprintf(chstr, 256, "%s, %d", meta_data->author, meta_data->cyear);
  mvwaddstr(footer, 0, maxx-strlen(chstr)-1, chstr);
  wrefresh(footer);
}

char *dsp_input(void) {
  static char str[25];
  wclear(input);
  mvwaddstr(input, 0, 1, ">> ");
  curs_set(1);
  mvwgetnstr(input, 0, 4, str, 24);
  curs_set(0);
  return str;
}

void dsp_output(char *str) {
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
          mvwdelch(output, line, tmppos);
          tmppos--;
          t--;
          len++;
        };
      }
      // got to next line if end of line has been reached
      line++;
      if (isspace(*(t))) {
        t++;
        pos++;
      }
      wmove(output, line, 1);
    }

    // test implementation, needs another implementation by using waddstr only!
    char wchars[] = "äüöÄÜÖ";
    if (strchr(wchars, *t) == 0) {
      waddch(output, *(t++));
    } else {
      char ch[2];
      snprintf(ch, 2, "%s", t++);
      waddstr(output, ch);
    }
    pos++;
  }
  wrefresh(output);
}
