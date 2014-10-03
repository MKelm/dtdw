#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "loader.h"

#define DEBUG

#define FILE_DESC "../def/descriptions.txt"

struct description {
  int id;
  char text[1024];
  int transitions[5];
  int items[10];
};
struct description desc_entries[1000];

int load_descriptions(void) {
  FILE *f = fopen(FILE_DESC, "r");
  int ch, entryidx = 0;
  // load status values
  int lroomid = 0, ltransid = 0, ltransididx = 0, litemsid = 0, litemsididx = 0;
  char line[1024] = "", chstr[2];
  // 0 = load room id, 1 = load text, 2 = load transitions and items
  int lroom = 0;

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
          desc_entries[entryidx].id = atoi(line);
          printf("\nroom id %d:\n", desc_entries[entryidx].id);
          strncpy(line, "", sizeof(line));
          ltransididx = 0;
          litemsididx = 0;
          lroom = 1;
        }
        break;
      case 1:
        // load room description
        if (ch != '\n') {
          snprintf(chstr, 2, "%c", ch);
          strcat(line, chstr);
        } else if (ch == '\n' && strlen(line) > 0) {
          strncpy(desc_entries[entryidx].text, line, sizeof(desc_entries[entryidx].text));
          printf("desc = %s\n ", desc_entries[entryidx].text);
          strncpy(line, "", sizeof(line));
          lroom = (desc_entries[entryidx].id == 0) ? 0 : 2;
        }
        break;
      case 2:
        // load transition ids
        if (ch == '#' && ltransid == 0) {
          ltransid = 1;
        } else if (ltransid == 1 && ch != '\n' && ch != '#' && ch != '$') {
          snprintf(chstr, 2, "%c", ch);
          strcat(line, chstr);
        } else if (ltransid == 1 && (ch == '\n' || ch == '#' || ch == '$')) {
          desc_entries[entryidx].transitions[ltransididx] = atoi(line);
          printf("trans %d = %d\n ", ltransididx, desc_entries[entryidx].transitions[ltransididx]);
          strncpy(line, "", sizeof(line));
          ltransididx++;
          ltransid = (ch == '#') ? 1 : 0;
        }
        // load item ids
        if (ch == '$' && litemsid == 0) {
          litemsid = 1;
        } else if (litemsid == 1 && ch != '\n' && ch != '$') {
          snprintf(chstr, 2, "%c", ch);
          strcat(line, chstr);
        } else if (litemsid == 1 && (ch == '\n' || ch == '$')) {
          desc_entries[entryidx].items[litemsididx] = atoi(line);
          printf("item %d = %s\n ", litemsididx, line);
          strncpy(line, "", sizeof(line));
          litemsididx++;
          litemsid = (ch == '$') ? 1 : 0;
        }
        // jump to next room description after transitions / items
        if (ch == '\n') {
          entryidx++;
          lroom = 0;
        }
        break;
    }
  }

  return ++entryidx; // returns entries count
}

#ifdef DEBUG

int main(void) {

  int i, entrycount = load_descriptions();

  /*printf("#####################\n");
  for (i = 0; i < entrycount; i++) {
    printf("desc id %d:\n%s\n%d, %d\n\n",
      desc_entries[i].id, desc_entries[i].text, desc_entries[i].transitions[0],
       desc_entries[i].items[0]
    );
  }*/

  return 0;
}

#endif