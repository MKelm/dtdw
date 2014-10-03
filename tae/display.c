#include <ncurses.h>
#include <string.h>
#include <locale.h>
#include <math.h>

#define DEBUG

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
  getch();
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
  mvwaddstr(header, 0, maxx-23, "Adventure demo, v0.0.1");
  wrefresh(header);

  wbkgd(footer, COLOR_PAIR(1));
  mvwaddstr(footer, 0, maxx-18, "Martin Kelm, 2014");
  wrefresh(footer);

  wbkgd(output, COLOR_PAIR(2));
  wrefresh(output);

  wbkgd(input, COLOR_PAIR(5));
  wattrset(input, A_BOLD);
  wrefresh(input);
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
  int line = 0, len = strlen(str), llen = maxx-3;
  line = (int)ceil(((double)len) / llen);
  printf("%d", line);
  while (len) {
    if (len > 1 && len % llen == 0) {
      line--;
    }
    wmove(output, line, 1);
    winsch(output, *(t+len-1));
    len--;
  }
  wrefresh(output);
}

#ifdef DEBUG

int main(void) {
  dsp_init();

  dsp_windows_init();

  char output[256] = "Something in the output window, and something other than my dog in window fight. But in all situations it is the best to perform a rabbit. it is like any other game here and there. Motivations are good to have so we go further and further.";
  dsp_output(output);
  char *input = dsp_input();

  dsp_end();
  return 0;
}

#endif