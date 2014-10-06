#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "loader.h"

void load_help(char *help) {
  FILE *f = fopen(FILE_HELP, "r");
  int ch;
  char chstr[2];

  while ((ch = fgetc(f)) != EOF) {
    snprintf(chstr, 2, "%c", ch);
    strcat(help, chstr);
  }
  fclose(f);
}

void load_meta(struct meta *data) {
  FILE *f = fopen(FILE_META, "r");
  int ch, linenum = 0;
  char line[1024] = "", chstr[2];

  while ((ch = fgetc(f)) != EOF) {
    if (ch != '\n') {
      snprintf(chstr, 2, "%c", ch);
      strcat(line, chstr);
    } else {
      if (linenum == 0)
        strncpy(data->title, line, sizeof(data->title));
      else if (linenum == 1)
        strncpy(data->version, line, sizeof(data->version));
      else if (linenum == 2)
        strncpy(data->author, line, sizeof(data->author));
      else if (linenum == 3)
        data->cyear = atoi(line);
      else if (linenum == 4)
        strncpy(data->tinventory, line, sizeof(data->tinventory));
      else
        strncpy(data->noinvitems, line, sizeof(data->noinvitems));
      strncpy(line, "", sizeof(line));
      linenum++;
    }
  }
  fclose(f);
}

int load_commands(struct command *data, int lmax) {
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
      strncpy(data[entryidx].in, line, sizeof(data[entryidx].in));
      strncpy(line, "", sizeof(line));
      loadin = 0;
      loadex = 1;
    } else if (loadex == 1 && ch == '\n') {
      strncpy(data[entryidx].ex, line, sizeof(data[entryidx].ex));
      strncpy(line, "", sizeof(line));
      loadex = 0;
      entryidx++;
      if (entryidx == lmax)
        return lmax;
    }
  }
  fclose(f);
  return entryidx;
}

int load_areas(struct area *data, int lmax) {
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
      data[entryidx].id = atoi(line);
      strncpy(line, "", sizeof(line));
      loadid = 0;
      loadtitle = 1;
    } else if (loadtitle == 1 && ch == '\n') {
      strncpy(data[entryidx].title, line, sizeof(data[entryidx].title));
      strncpy(line, "", sizeof(line));
      loadtitle = 0;
      entryidx++;
      if (entryidx == lmax)
        return lmax;
    }
  }
  fclose(f);
  return entryidx;
}

int load_places(struct place *data, int lmax) {
  FILE *f = fopen(FILE_PLACES, "r");
  int ch, entryidx = 0;
  char line[1024] = "", chstr[2];
  int loadareaid = 0, loadid = 0, loadtitle = 0;
  int loadtransitions = 0, loadtransid = 0, loadtranstitle = 0;
  int transitionidx = 0;

  while ((ch = fgetc(f)) != EOF) {
    if (loadareaid == 0 && loadid == 0 && loadtitle == 0 && loadtransitions == 0) {
      loadareaid = 1;
    }
    if ((loadareaid == 1 || loadid == 1 || loadtitle == 1 || loadtranstitle == 1) &&
        ch != '#' && ch != '\n') {
      snprintf(chstr, 2, "%c", ch);
      strcat(line, chstr);
    } else if (loadareaid == 1 && ch == '#') {
      data[entryidx].area_id = atoi(line);
      strncpy(line, "", sizeof(line));
      loadareaid = 0;
      loadid = 1;
    } else if (loadid == 1 && ch == '\n') {
      data[entryidx].id = atoi(line);
      strncpy(line, "", sizeof(line));
      loadid = 0;
      loadtitle = 1;
    } else if (loadtitle == 1 &&  ch == '\n') {
      strncpy(data[entryidx].title, line, sizeof(data[entryidx].title));
      strncpy(line, "", sizeof(line));
      loadtitle = 0;
      loadtransitions = 1;

    } else if (loadtransitions == 1) {
      if (loadtransid == 0 && loadtranstitle == 0 && ch != '#') {
        loadtransitions = 0;
        entryidx++;
        if (entryidx == lmax)
          return entryidx;
      } else if (loadtransid == 0 && loadtranstitle == 0 && ch == '#') {
        loadtransid = 1;
      } else if ((loadtransid == 1 && isdigit(ch)) ||
                 (loadtranstitle == 1 && ch != '#' && ch != '\n')) {
        snprintf(chstr, 2, "%c", ch);
        strcat(line, chstr);
      } else if (loadtransid == 1 && !isdigit(ch)) {
        data[entryidx].transitions[transitionidx].id = atoi(line);
        data[entryidx].transitions[transitionidx].locked = 0;
        strncpy(line, "", sizeof(line));
        loadtransid = 0;
        loadtranstitle = 1;
        snprintf(chstr, 2, "%c", ch);
        strcat(line, chstr);
      } else if (loadtranstitle == 1 && (ch == '#' || ch == '\n')) {
        strncpy(data[entryidx].transitions[transitionidx].title, line,
          sizeof(data[entryidx].transitions[transitionidx].title));
        strncpy(line, "", sizeof(line));
        loadtranstitle = 0;
        if (ch == '#') {
          loadtransid = 1;
          transitionidx++;
        } else {
          loadtransitions = 0;
          transitionidx = 0;
          entryidx++;
          if (entryidx == lmax)
            return entryidx;
        }
      }
    }
  }
  fclose(f);
  return entryidx;
}

int load_transitions(struct placetrans transitions_data[], int transitions_lmax,
                     struct place places_data[], int places_lmax, int placetrans_lmax) {
  int pidx, ptidx, tidx = 0;
  for (pidx = 0; pidx < places_lmax; pidx++) {
    ptidx = 0;
    for (ptidx = 0; ptidx < placetrans_lmax; ptidx++) {
      if (places_data[pidx].transitions[ptidx].id > 0) {
        transitions_data[tidx] = places_data[pidx].transitions[ptidx];
        ptidx++;
        tidx++;
        if (tidx == transitions_lmax)
          return tidx;
      }
    }
  }
  return tidx;
}

int load_items(struct item data[], int lmax) {
  FILE *f = fopen(FILE_ITEMS, "r");
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
        strncpy(data[entryidx].title, line, sizeof(data[entryidx].title));
        loadtitle = 0;
        entryidx++;
        if (entryidx == lmax)
          return lmax;
      } else {
        if (loadid == 1) {
          data[entryidx].id = atoi(line);
          data[entryidx].status = 0;
          loadid = 0;
          loadcombid = 1;
        } else if (loadcombid == 1) {
          data[entryidx].comb_id = atoi(line);
          loadcombid = 0;
          loadfinalid = 1;
        } else if (loadfinalid == 1) {
          data[entryidx].final_id = atoi(line);
          loadfinalid = 0;
          loadtitle = 1;
        }
        strncpy(line, "", sizeof(line));
      }
    }
  }
  fclose(f);
  return entryidx;
}

/* configuration style:
 &1 // npc id
 Marunix // npc title
 1#2 // npc area and place
*/
int load_npcs(struct npc data[], int lmax) {
  FILE *f = fopen(FILE_NPCS, "r");
  int ch, entryidx = 0;
  char line[1024] = "", chstr[2];
  int loadid = 0, loadtitle = 0, loadareaid = 0, loadplaceid = 0;

  while ((ch = fgetc(f)) != EOF) {
    if (ch == '&') {
      loadid = 1;
    } else if ((loadid == 1 || loadtitle == 1 || loadareaid == 1 || loadplaceid == 1) &&
               ch != '&' && ch != '#' && ch != '\n') {
      snprintf(chstr, 2, "%c", ch);
      strcat(line, chstr);
    } else if (ch == '&' || ch == '#' || ch == '\n') {
      if (loadtitle == 1) {
        strncpy(data[entryidx].title, line, sizeof(data[entryidx].title));
        strncpy(line, "", sizeof(line));
        loadtitle = 0;
        loadareaid = 1;
      } else if (loadid == 1) {
        data[entryidx].id = atoi(line);
        data[entryidx].cdialog = NULL;
        loadid = 0;
        loadtitle = 1;
      } else if (loadareaid == 1) {
        data[entryidx].area_id = atoi(line);
        loadareaid = 0;
        loadplaceid = 1;
      } else if (loadplaceid == 1) {
        data[entryidx].place_id = atoi(line);
        loadplaceid = 0;
        entryidx++;
        if (entryidx == lmax)
          return lmax;
      }
      strncpy(line, "", sizeof(line));
    }
  }
  fclose(f);
  return entryidx;
}

int load_dialogs(struct npc npcs_data[], int nlmax, struct dialog data[], int lmax) {
  int npcidx, titleidx, elementcount, elementidx, nextididx, ch, loadmode = 0;
  FILE *f;
  char line[1024] = "", title[56] = "", chstr[2];

  elementcount = 0;
  for (npcidx = 0; npcidx < nlmax; npcidx++) {
    strncpy(title, "", 56);
    if (npcidx == lmax)
      return npcidx;

    data[npcidx].npc_id = npcs_data[npcidx].id;
    for (titleidx = 0; npcs_data[npcidx].title[titleidx]; titleidx++){
      title[titleidx] = tolower(npcs_data[npcidx].title[titleidx]);
    }
    snprintf(line, 1024, "%s%s%s", FILE_DIALOGS_FOLDER, title, FILE_DIALOGS_POSTFIX);

    elementidx = 0;
    nextididx = -1;
    f = fopen(line, "r");
    strncpy(line, "", 1024);

    // loadmode 0 = nothing, 1 = id, 2 = text, 3 = next ids
    while ((ch = fgetc(f)) != EOF) {
      if (loadmode == 0 && ch == '!') {
        loadmode = 1;
      } else if (loadmode == 1 && ch == '\n') {
        data[npcidx].elements[elementidx].id = atoi(line);
        strncpy(line, "", sizeof(line));
        loadmode = 2;
      } else if (loadmode == 2 && ch == '\n') {
        strncpy(data[npcidx].elements[elementidx].text, line,
          sizeof(data[npcidx].elements[elementidx].text));
        strncpy(line, "", sizeof(line));
        loadmode = 3;
      } else if (loadmode == 3 && (ch == '!' || ch == '?' || ch == '\n')) {
        if (nextididx == -1 && ch != '\n') {
          nextididx++;
        } else if (nextididx == -1 && ch == '\n') {
          loadmode = 0;
          elementidx++;
        } else {
          data[npcidx].elements[elementidx].next_ids[nextididx] = atoi(line);
          if (ch != '\n')
            data[npcidx].elements[elementidx].next_mchoice = (ch == '?') ? 1 : 0;
          strncpy(line, "", sizeof(line));
          nextididx++;
          if (ch == '\n') {
            loadmode = 0;
            nextididx = -1;
            elementidx++;
          }
        }
      } else if (loadmode == 1 || loadmode == 2 || loadmode == 3) {
        snprintf(chstr, 2, "%c", ch);
        strcat(line, chstr);
      }
    }
    if (data[npcidx].elements[elementidx].id > 0)
      elementidx++;
    elementcount += elementidx;
    fclose(f);
  }
  return elementcount;
}
/*
 Description definitions:
 #placeId$itemId...(optional)/verb(optional)
 Text with #transition# or $item$ ...
 #transitionID...$itemID...
*/
int load_descriptions(struct description data[], int lmax) {
  FILE *f = fopen(FILE_DESCRIPTIONS, "r");
  int ch, entryidx = 0;
  // load status values: main id, id trans id, id items id, id verb, transition id, items id
  int loadid = 0, lidtransid = 0, liditemsid = 0, lidverb = 0, ltransid = 0, litemsid = 0;
  // transition / item ids indexes
  int lidtransidx = 0, liditemidx = 0, ltransidx = 0, litemidx = 0;
  // temporary characters
  char line[1024] = "", chstr[2];
  // load modes: 0 = id (+ desc items) (+ verb), 1 = text, 2 = transitions and items
  int loadmode = 0;

  while ((ch = fgetc(f)) != EOF) {
    switch (loadmode) {
      case 0:
        // load desc id
        if (ch == '#' && loadid == 0 && lidtransid == 0) {
          loadid = 1;
        } else if (loadid == 1 && ch != '\n' && ch != '#' && ch != '$' && ch != '/') {
          snprintf(chstr, 2, "%c", ch);
          strcat(line, chstr);
        } else if (loadid == 1 && (ch == '\n' || ch == '#' || ch == '$' || ch == '/')) {
          loadid = 0;
          data[entryidx].id = atoi(line);
          strncpy(line, "", sizeof(line));
          liditemidx = 0;
          lidtransidx = 0;
          ltransidx = 0;
          litemidx = 0;
          if (ch == '\n')
            loadmode = 1;
        }
        // load id transition ids
        if (ch == '#' && loadid == 0 && lidtransid == 0) {
          lidtransid = 1;
        } else if (lidtransid == 1 && ch != '\n' && ch != '#' && ch != '$' && ch != '/') {
          snprintf(chstr, 2, "%c", ch);
          strcat(line, chstr);
        } else if (lidtransid == 1 && (ch == '\n' || ch == '#' || ch == '$' || ch == '/')) {
          data[entryidx].id_transitions[lidtransidx] = atoi(line);
          strncpy(line, "", sizeof(line));
          lidtransidx++;
          lidtransid = (ch == '#') ? 1 : 0;
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
          data[entryidx].id_items[liditemidx] = atoi(line);
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
          strncpy(data[entryidx].id_verb, line, sizeof(data[entryidx].id_verb));
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
          strncpy(data[entryidx].text, line, sizeof(data[entryidx].text));
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
          data[entryidx].transitions[ltransidx] = atoi(line);
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
          data[entryidx].items[litemidx] = atoi(line);
          strncpy(line, "", sizeof(line));
          litemidx++;
          litemsid = (ch == '$') ? 1 : 0;
        }
        // jump to next room description after transitions / items
        if (ch == '\n') {
          entryidx++;
          if (entryidx == lmax) {
            return entryidx;
          }
          loadmode = 0;
        }
        break;
    }
  }
  if (data[entryidx].id > 0) {
    entryidx++;
  }
  fclose(f);
  return entryidx; // returns entries count
}
