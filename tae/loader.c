#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "loader.h"

void load_help(char *help) {
  int ch, ch_count = 0;
  char ch_str[2];
  FILE *f = fopen(FILE_HELP, "r");
  while ((ch = fgetc(f)) != EOF && ch_count < MAX_HELP_TEXT_CHARS) {
    snprintf(ch_str, 2, "%c", ch);
    strcat(help, ch_str);
    ch_count++;
  }
  fclose(f);
}

void load_meta(struct meta *data) {
  int linenum = 0, run = 1;
  char line[1024];
  FILE *f = fopen(FILE_META, "r");
  do {
    strncpy(line, "", sizeof(line));
    if (fscanf(f, "%[^\n]\n", &line[0]) && strlen(line) > 0) {
      switch (linenum) {
        case 0:
          strncpy(data->title, line, sizeof(data->title));
          break;
        case 1:
          strncpy(data->version, line, sizeof(data->version));
          break;
        case 2:
          strncpy(data->author, line, sizeof(data->author));
          break;
        case 3:
          data->cyear = atoi(line);
          break;
      }
      linenum++;
    } else {
      run = 0;
    }
  } while (run == 1);
  fclose(f);
}

void load_phrases(struct phrases *data) {
  int linenum = 0, run = 1;
  char line[1024];
  FILE *f = fopen(FILE_PHRASES, "r");
  do {
    strncpy(line, "", sizeof(line));
    if (fscanf(f, "%[^\n]\n", &line[0]) && strlen(line) > 0) {
      switch (linenum) {
        case 0:
          strncpy(data->inv_title, line, sizeof(data->inv_title));
          break;
        case 1:
          strncpy(data->no_inv_items, line, sizeof(data->no_inv_items));
          break;
        case 2:
          strncpy(data->items_comb, line, sizeof(data->items_comb));
          break;
        case 3:
          strncpy(data->items_comb_failure, line, sizeof(data->items_comb_failure));
          break;
        case 4:
          strncpy(data->item_usage_failure, line, sizeof(data->item_usage_failure));
          break;
      }
      linenum++;
    } else {
      run = 0;
    }
  } while (run == 1);
  fclose(f);
}

int load_commands(struct command *data, int lmax) {
  int data_idx = 0, run = 1;
  char intern_command[MAX_COMMAND_LENGTH], extern_command[MAX_COMMAND_LENGTH];
  FILE *f = fopen(FILE_COMMANDS, "r");
  do {
    strncpy(intern_command, "", sizeof(intern_command));
    strncpy(extern_command, "", sizeof(extern_command));
    if (fscanf(f, "%[^=]=%[^\n]\n", &intern_command[0], &extern_command[0]) &&
        strlen(intern_command) > 0 && strlen(extern_command) > 0) {
      strncpy(data[data_idx].in, intern_command, sizeof(data[data_idx].in));
      strncpy(data[data_idx].ex, extern_command, sizeof(data[data_idx].ex));
      data_idx++;
    } else {
      run = 0;
    }
  } while (run == 1);
  fclose(f);
  return data_idx;
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

int load_places_rec(FILE *f, struct place *data, int data_idx) {
  int area_id, place_id, transition_id;
  char title[256] = "", transition_title[256] = "", trans_status[2] = "";

  if (fscanf(f, "%d#%d\n", &area_id, &place_id)) {
    data[data_idx].area_id = area_id;
    data[data_idx].id = place_id;
  }
  if (area_id > 0 && place_id > 0) {
    if (fscanf(f, "%[^\n]\n", &title[0])) {
      strncpy(data[data_idx].title, title, sizeof(data[data_idx].title));
    }
    // todo: add multiple transitions support if needed
    if (fscanf(f, "#%d%[^<^\n]", &transition_id, &transition_title[0])) {
      data[data_idx].transitions[0].id = transition_id;
      strncpy(data[data_idx].transitions[0].title, transition_title,
        sizeof(data[data_idx].transitions[0].title));
    }
    if (fgetc(f) != '\n') {
      fscanf(f, "%[^#^\n^$]", &trans_status[0]);
      data[data_idx].transitions[0].status = 0; // open
      if (strcmp(trans_status, "c") == 0) {
        data[data_idx].transitions[0].status = 1; // closed
      } else if (strcmp(trans_status, "l") == 0) {
        data[data_idx].transitions[0].status = 2; // locked
        fscanf(f, "$%d", &data[data_idx].transitions[0].ul_item_id);
      }
      if (fgetc(f) == '\n') {
        data_idx++;
        data_idx = load_places_rec(f, data, data_idx);
      }
    }
  }
  return data_idx;
}

int load_places(struct place *data, int lmax) {
  FILE *f = fopen(FILE_PLACES, "r");
  int data_idx = 0;
  data_idx = load_places_rec(f, data, data_idx);
  fclose(f);
  return ++data_idx;
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
        data[entryidx].c_dialog = NULL;
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
      } else if (loadmode == 3 && (ch == '>' || ch == '?' || ch == '\n')) {
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
    data[npcidx].elements_count = elementidx;
    elementcount += elementidx;
    fclose(f);
    // link dialog to npc
    npcs_data[npcidx].c_dialog = &data[npcidx];
  }
  return elementcount;
}

/*
 Description definitions:
 #placeId&npcId$itemId...(optional)/verb(optional)
 Text with #transition# or &npc& or $item$ ...
 #transitionIDs...npcIDs...$itemIDs...
*/
int load_descriptions_rec(FILE *f, struct description *data, int data_idx) {
  char str[256];
  int tmp, ch;

  if (fscanf(f, "#%d", &data[data_idx].id)) {
    // switch to get id transitions / id npcs or id items
    if ((ch = fgetc(f)) != '\n') {
      int id_trans_idx = 0, id_npc_idx = 0, id_item_idx = 0;
      do {
        fseek(f, ftell(f) - 1, SEEK_SET);
        fscanf(f, "%[#&$]%d", str, &tmp);
        if (strcmp(str, "#") == 0 && id_trans_idx < MAX_DESC_ID_EXTRAS) {
          data[data_idx].id_transitions[id_trans_idx] = tmp;
          data[data_idx].id_trans_status[id_trans_idx] = 0;
          // id trans status used to get descriptions in relation to closed / locked status
          if ((ch = fgetc(f)) == '<') {
            fscanf(f, "%[^\n^/^$]", str);
            if (strcmp(str, "c") == 0) {
              data[data_idx].id_trans_status[id_trans_idx] = 1; // closed
              data[data_idx].id_trans_item_id[id_trans_idx] = 0;
            } else if (strcmp(str, "l") == 0) {
              data[data_idx].id_trans_status[id_trans_idx] = 2; // locked
              // needs an item (id) to unlock transition
              fscanf(f, "$%d", &data[data_idx].id_trans_item_id[id_trans_idx]);
            }
          } else {
            fseek(f, ftell(f) - 1, SEEK_SET);
          }
          id_trans_idx++;
        } else if (strcmp(str, "&") == 0 && id_npc_idx < MAX_DESC_ID_EXTRAS) {
          data[data_idx].id_npcs[id_npc_idx] = tmp;
          id_npc_idx++;
        } else if (strcmp(str, "$") == 0 && id_item_idx < MAX_DESC_ID_EXTRAS) {
          data[data_idx].id_items[id_item_idx] = tmp;
          id_item_idx++;
        }
      } while ((ch = fgetc(f)) != '\n' && ch != '/');
    }
    // command detection
    if (ch != '\n' && ch == '/') {
      fscanf(f, "%[^\n]\n", data[data_idx].id_verb);
    }
    // get text
    fgets(data[data_idx].text, sizeof(data[data_idx].text), f);
    int len = strlen(data[data_idx].text);
    if (data[data_idx].text[len - 1] == '\n')
        data[data_idx].text[len - 1] = '\0';
    // switch to get transitions / npcs or items
    if ((ch = fgetc(f)) != '\n') {
      int trans_idx = 0, npc_idx = 0, item_idx = 0;
      do {
        strncpy(str, "", sizeof(str));
        fseek(f, ftell(f) - 1, SEEK_SET);
        fscanf(f, "%[#&$]%d", str, &tmp);
        if (strcmp(str, "#") == 0 && trans_idx < MAX_DESC_TEXT_TRANS) {
          data[data_idx].transitions[trans_idx] = tmp;
          trans_idx++;
        } else if (strcmp(str, "&") == 0 && npc_idx < MAX_DESC_TEXT_NPCS) {
          data[data_idx].npcs[npc_idx] = tmp;
          npc_idx++;
        } else if (strcmp(str, "$") == 0 && item_idx < MAX_DESC_TEXT_ITEMS) {
          data[data_idx].items[item_idx] = tmp;
          item_idx++;
        }
      } while ((ch = fgetc(f)) != '\n');
    } else {
      fseek(f, ftell(f) - 1, SEEK_SET);
    }
    // search for more entries to get data from
    while ((ch = fgetc(f)) == '\n');
    if (ch == '#') {
      fseek(f, ftell(f) - 1, SEEK_SET);
      data_idx++;
      data_idx = load_descriptions_rec(f, data, data_idx);
    }
  }

  return data_idx;
}

int load_descriptions(struct description *data, int lmax) {
  FILE *f = fopen(FILE_DESCRIPTIONS, "r");
  int data_idx = 0;
  data_idx = load_descriptions_rec(f, data, data_idx);
  fclose(f);
  return ++data_idx;
}
