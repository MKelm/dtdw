#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define DEBUG

#define FILE_DESC "../def/descriptions.txt"

void load_descriptions(void) {
  FILE *f = fopen(FILE_DESC, "r");
  int ch, aroom = -1, lroom = 0;
  char line[1024] = "", chstr[2];

  while ((ch = fgetc(f)) != EOF) {
    // load room string
    if (aroom == -1) {
      if (ch == '#' && lroom == 0) {
        lroom = 1;
      } else if (lroom == 1 && ch != '\n') {
        snprintf(chstr, 2, "%c", ch);
        strcat(line, chstr);
      } else if (lroom == 1 && ch == '\n') {
        lroom = 0;
        aroom = atoi(line);
        strncpy(line, "", sizeof(line));
      }
    } else {
      // load room description
      if (ch != '\n') {
        snprintf(chstr, 2, "%c", ch);
        strcat(line, chstr);
      } else if (ch == '\n') {
        printf("\n\nroom %d\n%s\n", aroom, line);
        aroom = -1;
      }
    }
  }
}

#ifdef DEBUG

int main(void) {

  load_descriptions();

  return 0;
}

#endif