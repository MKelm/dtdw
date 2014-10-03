#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "loader.h"

#define DEBUG

/*
 Description definitions:
 #placeId$itemId...(optional)/verb(optional)
 Text with #transition# or $item$ ...
 #transitionID...$itemID...
*/
int load_descriptions(struct description descriptions[], int lmax) {

  FILE *f = fopen(FILE_DESCRIPTIONS, "r");
  int ch, entryidx = 0;
  // load status values: main id, id items id, id verb, transition id, items id
  int loadid = 0, liditemsid = 0, lidverb = 0, ltransid = 0, litemsid = 0;
  // transition / item ids indexes
  int liditemidx = 0, ltransidx = 0, litemidx = 0;
  // temporary characters
  char line[1024] = "", chstr[2];
  // load modes: 0 = id (+ desc items) (+ verb), 1 = text, 2 = transitions and items
  int loadmode = 0;

  while ((ch = fgetc(f)) != EOF) {
    switch (loadmode) {
      case 0:
        // load desc id
        if (ch == '#' && loadid == 0) {
          loadid = 1;
        } else if (loadid == 1 && ch != '\n' && ch != '$' && ch != '/') {
          snprintf(chstr, 2, "%c", ch);
          strcat(line, chstr);
        } else if (loadid == 1 && (ch == '\n' || ch == '$' || ch == '/')) {
          loadid = 0;
          descriptions[entryidx].id = atoi(line);
          //printf("\nroom id %d:\n", descriptions[entryidx].id);
          strncpy(line, "", sizeof(line));
          ltransidx = 0;
          litemidx = 0;
          if (ch == '\n')
            loadmode = 1;
        }
        // load id item ids
        if (ch == '$' && liditemsid == 0) {
          liditemsid = 1;
        } else if (liditemsid == 1 && ch != '\n' && ch != '$' && ch != '/') {
          snprintf(chstr, 2, "%c", ch);
          strcat(line, chstr);
        } else if (liditemsid == 1 && (ch == '\n' || ch == '$' || ch == '/')) {
          descriptions[entryidx].id_items[liditemidx] = atoi(line);
          //printf("id item %d = %s\n ", liditemidx, line);
          strncpy(line, "", sizeof(line));
          liditemidx++;
          liditemsid = (ch == '$') ? 1 : 0;
          if (ch == '\n')
            loadmode = 1;
        }
        // load id verb ids
        if (ch == '/' && lidverb == 0) {
          lidverb = 1;
        } else if (lidverb == 1 && ch != '\n') {
          snprintf(chstr, 2, "%c", ch);
          strcat(line, chstr);
        } else if (lidverb == 1 && ch == '\n') {
          strncpy(descriptions[entryidx].id_verb, line, sizeof(descriptions[entryidx].id_verb));
          //printf("id verb = %s\n ", descriptions[entryidx].id_verb);
          strncpy(line, "", sizeof(line));
          lidverb = 0;
          loadmode = 1;
        }
        break;
      case 1:
        // load room description
        if (ch != '\n') {
          snprintf(chstr, 2, "%c", ch);
          strcat(line, chstr);
        } else if (ch == '\n' && strlen(line) > 0) {
          strncpy(descriptions[entryidx].text, line, sizeof(descriptions[entryidx].text));
          //printf("desc = %s\n ", descriptions[entryidx].text);
          strncpy(line, "", sizeof(line));
          loadmode = 2;
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
          descriptions[entryidx].transitions[ltransidx] = atoi(line);
          //printf("trans %d = %d\n ", ltransidx, descriptions[entryidx].transitions[ltransidx]);
          strncpy(line, "", sizeof(line));
          ltransidx++;
          ltransid = (ch == '#') ? 1 : 0;
        }
        // load item ids
        if (ch == '$' && litemsid == 0) {
          litemsid = 1;
        } else if (litemsid == 1 && ch != '\n' && ch != '$') {
          snprintf(chstr, 2, "%c", ch);
          strcat(line, chstr);
        } else if (litemsid == 1 && (ch == '\n' || ch == '$')) {
          descriptions[entryidx].items[litemidx] = atoi(line);
          //printf("item %d = %s\n ", litemidx, line);
          strncpy(line, "", sizeof(line));
          litemidx++;
          litemsid = (ch == '$') ? 1 : 0;
        }
        // jump to next room description after transitions / items
        if (ch == '\n') {
          entryidx++;
          if (entryidx == lmax)
            return entryidx;
          loadmode = 0;
        }
        break;
    }
  }

  return entryidx; // returns entries count
}

#ifdef DEBUG

int main(void) {

  struct description descriptions[1000];
  int i, desc_count = load_descriptions(descriptions, 1000);

  for (i = 0; i < desc_count; i++) {
    printf("desc id %d:\n%s\n%d, %d\n\n",
      descriptions[i].id, descriptions[i].text, descriptions[i].transitions[0],
       descriptions[i].items[0]
    );
  }

  return 0;
}

#endif