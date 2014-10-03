#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define DEBUG

#define FILE_DESC "../def/descriptions.txt"

void load_descriptions(void) {
  FILE *f = fopen(FILE_DESC, "r");
  int ch, lroomid = 0, ltransid = 0, litemsid = 0;
  char line[1024] = "", chstr[2];
  // 0 = load id, 1 = load text, 2 = load transitions, 3 = load items
  int lroom = 0;

  struct description {
    int id;
    char text[1024];
    int transitions[5];
    int items[10];
  } desc_entry;

  while ((ch = fgetc(f)) != EOF) {
    switch (lroom) {
      case 0:
        // load room id
        // todo: load room id with item / verb combination
        if (ch == '#' && lroomid == 0) {
          lroomid = 1;
        } else if (lroomid == 1 && ch != '\n') {
          snprintf(chstr, 2, "%c", ch);
          strcat(line, chstr);
        } else if (lroomid == 1 && ch == '\n') {
          lroomid = 0;
          desc_entry.id = atoi(line);
          strncpy(line, "", sizeof(line));
          lroom = 1;
        }
        break;
      case 1:
        // load room description
        if (ch != '\n') {
          snprintf(chstr, 2, "%c", ch);
          strcat(line, chstr);
        } else if (ch == '\n' && strlen(line) > 0) {
          strncpy(desc_entry.text, line, sizeof(desc_entry.text));

          printf("\n\nroom %d\n%s\n", desc_entry.id, desc_entry.text);
          strncpy(line, "", sizeof(line));
          lroom = 2;
        }
        break;
      case 2:
      case 3:
        // load transition ids
        if (ch == '#' && ltransid == 0) {
          ltransid = 1;
        } else if (ltransid == 1 && ch != '\n' && ch != '#') {
          snprintf(chstr, 2, "%c", ch);
          strcat(line, chstr);
        } else if (ltransid == 1 && (ch == '\n' || ch == '#')) {
          printf("\nroom %d, transid %d", desc_entry.id, atoi(line));
          strncpy(line, "", sizeof(line));
          if (ch != '#') {
            ltransid = 0;
            lroom = 3;
          }
        // load item ids
        } else if (ch == '$' && litemsid == 0) {
          litemsid = 1;
        } else if (litemsid == 1 && ch != '\n' && ch != '$') {
          snprintf(chstr, 2, "%c", ch);
          strcat(line, chstr);
        } else if (litemsid == 1 && (ch == '\n' || ch == '$')) {
          printf("\nroom %d, itemid %d", desc_entry.id, atoi(line));
          strncpy(line, "", sizeof(line));
          if (ch != '$') {
            litemsid = 0;
            lroom = 0;
          }

        } else {
          lroom = 0;
        }
        break;
    }
  }
}

#ifdef DEBUG

int main(void) {

  load_descriptions();

  return 0;
}

#endif