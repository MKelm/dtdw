#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "loader.h"

#define DEBUG

void load_meta(struct meta *meta_data) {
  FILE *f = fopen(FILE_META, "r");
  int ch, linenum = 0;
  char line[1024] = "", chstr[2];

  while ((ch = fgetc(f)) != EOF) {
    if (ch != '\n') {
      snprintf(chstr, 2, "%c", ch);
      strcat(line, chstr);
    } else {
      if (linenum == 0)
        strncpy(meta_data->title, line, sizeof(meta_data->title));
      else if (linenum == 1)
        strncpy(meta_data->version, line, sizeof(meta_data->version));
      else if (linenum == 2)
        strncpy(meta_data->author, line, sizeof(meta_data->author));
      else
        meta_data->cyear = atoi(line);
      strncpy(line, "", sizeof(line));
      linenum++;
    }
  }
}

int load_commands(struct command *commands, int lmax) {
  FILE *f = fopen(FILE_COMMANDS, "r");
  int ch, entryidx = 0;
  char line[1024] = "", chstr[2];
  int loadin = 0, loadex = 0;

  while ((ch = fgetc(f)) != EOF) {
    if (ch != '\n' && loadin == 0 && loadex == 0) {
      loadin = 1;
    }
    if ((loadin == 1 || loadex == 1) && ch != '\n' && ch != '=') {
      snprintf(chstr, 2, "%c", ch);
      strcat(line, chstr);
    } else if (loadin == 1 && ch == '=') {
      strncpy(commands[entryidx].in, line, sizeof(commands[entryidx].in));
      strncpy(line, "", sizeof(line));
      loadin = 0;
      loadex = 1;
    } else if (loadex == 1 && ch == '\n') {
      strncpy(commands[entryidx].ex, line, sizeof(commands[entryidx].ex));
      strncpy(line, "", sizeof(line));
      loadex = 0;
      entryidx++;
      if (entryidx == lmax)
        return lmax;
    }
  }

  return entryidx;
}

int load_areas(struct area *areas, int lmax) {
  FILE *f = fopen(FILE_AREAS, "r");
  int ch, entryidx = 0;
  char line[1024] = "", chstr[2];
  int loadid = 0, loadtitle = 0;

  while ((ch = fgetc(f)) != EOF) {
    if (loadid == 0 && loadtitle == 0) {
      loadid = 1;
    }
    if ((loadid == 1 || loadtitle == 1) && ch != '\n') {
      snprintf(chstr, 2, "%c", ch);
      strcat(line, chstr);
    } else if (loadid == 1 && ch == '\n') {
      areas[entryidx].id = atoi(line);
      strncpy(line, "", sizeof(line));
      loadid = 0;
      loadtitle = 1;
    } else if (loadtitle == 1 && ch == '\n') {
      strncpy(areas[entryidx].title, line, sizeof(areas[entryidx].title));
      strncpy(line, "", sizeof(line));
      loadtitle = 0;
      entryidx++;
      if (entryidx == lmax)
        return lmax;
    }
  }

  return entryidx;
}

int load_places(struct place *places, int lmax) {
  FILE *f = fopen(FILE_PLACES, "r");
  int ch, entryidx = 0;
  char line[1024] = "", chstr[2];
  int loadareaid = 0, loadid = 0, loadtitle = 0;

  while ((ch = fgetc(f)) != EOF) {
    if (loadareaid == 0 && loadid == 0 && loadtitle == 0) {
      loadareaid = 1;
    }
    if ((loadareaid == 1 || loadid == 1 || loadtitle == 1) && ch != '#' && ch != '\n') {
      snprintf(chstr, 2, "%c", ch);
      strcat(line, chstr);
    } else if (loadareaid == 1 && ch == '#') {
      places[entryidx].area_id = atoi(line);
      strncpy(line, "", sizeof(line));
      loadareaid = 0;
      loadid = 1;
    } else if (loadid == 1 && ch == '\n') {
      places[entryidx].id = atoi(line);
      strncpy(line, "", sizeof(line));
      loadid = 0;
      loadtitle = 1;
    } else if (loadtitle == 1 &&  ch == '\n') {
      strncpy(places[entryidx].title, line, sizeof(places[entryidx].title));
      strncpy(line, "", sizeof(line));
      loadtitle = 0;
      entryidx++;
      if (entryidx == lmax)
        return lmax;
    }
  }

  return entryidx;
}

int load_objects(struct object objects[], int lmax) {
  FILE *f = fopen(FILE_OBJECTS, "r");
  int ch, entryidx = 0;
  char line[1024] = "", chstr[2];
  int loadid = 0, loadcombid = 0, loadfinalid = 0, loadtitle = 0;

  while ((ch = fgetc(f)) != EOF) {
    if (ch == '$') {
      loadid = 1;
    } else if ((loadid == 1 || loadcombid == 1 || loadfinalid == 1 || loadtitle == 1) &&
               ch != '&' && ch != '=' && ch != '\n') {
      snprintf(chstr, 2, "%c", ch);
      strcat(line, chstr);
    } else if (ch == '&' || ch == '=' || ch == '\n') {
      if (loadtitle == 1) {
        strncpy(objects[entryidx].title, line, sizeof(objects[entryidx].title));
        loadtitle = 0;
        entryidx++;
        if (entryidx == lmax)
          return lmax;
      } else {
        if (loadid == 1) {
          objects[entryidx].id = atoi(line);
          loadid = 0;
          loadcombid = 1;
        } else if (loadcombid == 1) {
          objects[entryidx].comb_id = atoi(line);
          loadcombid = 0;
          loadfinalid = 1;
        } else if (loadfinalid == 1) {
          objects[entryidx].final_id = atoi(line);
          loadfinalid = 0;
          loadtitle = 1;
        }
        strncpy(line, "", sizeof(line));
      }
    }
  }

  return entryidx;
}

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
