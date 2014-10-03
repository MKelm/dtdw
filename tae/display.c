#include <ncurses.h>
#include <locale.h>

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
  mvwaddstr(header, 0, 1, "Area one, place tow");
  mvwaddstr(header, 0, maxx-23, "Adventure demo, v0.0.1");
  wrefresh(header);

  wbkgd(footer, COLOR_PAIR(1));
  mvwaddstr(footer, 0, maxx-18, "Martin Kelm, 2014");
  wrefresh(footer);

  wbkgd(output, COLOR_PAIR(2));
  mvwaddstr(output, 1, 1, "Something in the output window");
  wrefresh(output);

  wbkgd(input, COLOR_PAIR(5));
  mvwaddstr(input, 0, 1, ">> ");
  wrefresh(input);
}

#ifdef DEBUG

int main(void) {
  dsp_init();

  dsp_windows_init();

  dsp_end();
  return 0;
}

#endif