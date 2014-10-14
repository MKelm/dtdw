#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include "loader.h"

int current_area_id = 0;
char data_directory[256];

void loader_set_data_dir(void) {
  ssize_t len = readlink("/proc/self/exe", data_directory, sizeof(data_directory));
  if (len != -1)
    data_directory[len-7] = '\0';
  strcat(data_directory, LOADER_DATA_FOLDER);
}

void loader_set_area_id(int area_id) {
  current_area_id = area_id;
}

FILE *loader_get_data_file(char *file_name, short in_area) {
  char tmp1[256], tmp2[256];
  strncpy(tmp1, data_directory, 256);
  if (in_area == 1) {
    strcat(tmp1, "a");
    if (current_area_id < 10) {
      strcat(tmp1, "00");
    } else if (current_area_id < 100) {
      strcat(tmp1, "0");
    }
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "%d/", current_area_id);
    strcat(tmp1, buffer);
  }
  strncpy(tmp2, file_name, 256);
  strcat(tmp1, tmp2);
  return fopen(tmp1, "r");
}

void load_json(FILE *f, char *output, int output_length, jsmntok_t *tokens, int tokens_length) {
  int ch, ch_count = 0, new_line = 1;
  char ch_str[2];
  strcpy(output, "");
  while ((ch = fgetc(f)) != EOF && ch_count < output_length) {
    if (ch == '\n') {
      strcat(output, " ");
      new_line = 1;
    } else if (new_line == 0 || ch != ' ') {
      snprintf(ch_str, 2, "%c", ch);
      strcat(output, ch_str);
      new_line = 0;
    }
    ch_count++;
  }
  int r;
  jsmn_parser p;
  jsmn_init(&p);
  r = jsmn_parse(&p, output, strlen(output), tokens, tokens_length);
  if (r < 0) {
    tokens = NULL;
  }
}

void load_json_token(char* input, char *token, jsmntok_t *tokens, int token_idx) {
  strncpy(token, "", MAX_JSON_LINE_CHARS);
  strncpy(token, input + tokens[token_idx].start,
    tokens[token_idx].end - tokens[token_idx].start);
}

void load_help(char *help) {
  FILE *f = loader_get_data_file(FILE_HELP, 0);

  char output[MAX_HELP_TEXT_CHARS];
  jsmntok_t tokens[128];
  load_json(f, output, 2048, tokens, 128);

  int i = 1, has_help_array = 0;
  char line[MAX_JSON_LINE_CHARS];
  strcpy(help, "");
  while (tokens[i].end != 0 && tokens[i].end < tokens[0].end) {
    if (has_help_array == 1 && tokens[i].type == JSMN_STRING) {
      load_json_token(output, line, tokens, i);
      strcat(line, "\n");
      strcat(help, line);
    } else if (tokens[i].type == JSMN_ARRAY && tokens[i].size > 0) {
      i++;
      has_help_array = 1;
    }
    i++;
  }

  fclose(f);
}

void load_meta(struct meta *data) {
  FILE *f = loader_get_data_file(FILE_META, 0);

  char output[2048];
  jsmntok_t tokens[128];
  load_json(f, output, 2048, tokens, 128);

  int i = 1;
  char line[MAX_JSON_LINE_CHARS];
  while (tokens[i].end != 0 && tokens[i].end < tokens[0].end) {

    if (tokens[i].type == JSMN_STRING) {
      strncpy(line, "", sizeof(line));
      strncpy(line, output + tokens[i].start, tokens[i].end - tokens[i].start);
      i++;
      if (strcmp(line, "title") == 0) {
        load_json_token(output, line, tokens, i);
        strncpy(data->title, line, sizeof(data->title));
      } else if (strcmp(line, "version") == 0) {
        load_json_token(output, line, tokens, i);
        strncpy(data->version, line, sizeof(data->version));
      } else if (strcmp(line, "author") == 0) {
        load_json_token(output, line, tokens, i);
        strncpy(data->author, line, sizeof(data->author));
      } else if (strcmp(line, "year") == 0) {
        load_json_token(output, line, tokens, i);
        data->cyear = atoi(line);
      }
    }
    if (tokens[i].end >= tokens[0].end)
      break;
    i++;
  }

  fclose(f);
}

void load_phrases(struct phrases *data) {
  FILE *f = loader_get_data_file(FILE_PHRASES, 0);

  char output[2048];
  jsmntok_t tokens[128];
  load_json(f, output, 2048, tokens, 128);

  int i = 1;
  char line[MAX_JSON_LINE_CHARS];
  while (tokens[i].end != 0 && tokens[i].end < tokens[0].end) {

    if (tokens[i].type == JSMN_STRING) {
      strncpy(line, "", sizeof(line));
      strncpy(line, output + tokens[i].start, tokens[i].end - tokens[i].start);
      i++;
      if (tokens[i].type == JSMN_STRING) {
        if (strcmp(line, "inv_title") == 0) {
          load_json_token(output, line, tokens, i);
          strncpy(data->inv_title, line, sizeof(data->inv_title));
        } else if (strcmp(line, "no_inv_items") == 0) {
          load_json_token(output, line, tokens, i);
          strncpy(data->no_inv_items, line, sizeof(data->no_inv_items));
        } else if (strcmp(line, "items_comb") == 0) {
          load_json_token(output, line, tokens, i);
          strncpy(data->items_comb, line, sizeof(data->items_comb));
        } else if (strcmp(line, "items_comb_failure") == 0) {
          load_json_token(output, line, tokens, i);
          strncpy(data->items_comb_failure, line, sizeof(data->items_comb_failure));
        } else if (strcmp(line, "item_usage_failure") == 0) {
          load_json_token(output, line, tokens, i);
          strncpy(data->item_usage_failure, line, sizeof(data->item_usage_failure));
        }
      }
    }
    if (tokens[i].end >= tokens[0].end)
      break;
    i++;
  }

  fclose(f);
}

int load_commands(struct command *data, int lmax) {
  FILE *f = loader_get_data_file(FILE_COMMANDS, 0);

  char output[2048];
  jsmntok_t tokens[128];
  load_json(f, output, 2048, tokens, 128);

  int i = 1, idx = 0;
  char line[MAX_JSON_LINE_CHARS];
  while (tokens[i].end != 0 && tokens[i].end < tokens[0].end) {

    if (tokens[i].type == JSMN_STRING) {
      load_json_token(output, line, tokens, i++);
      if (strlen(line) > 0 && tokens[i].type == JSMN_STRING) {
        strncpy(data[idx].in, line, sizeof(data[idx].in));
        load_json_token(output, line, tokens, i);
        strncpy(data[idx].ex, line, sizeof(data[idx].ex));
        idx++;
      }
    }
    if (tokens[i].end >= tokens[0].end)
      break;
    i++;
  }

  fclose(f);
  return idx;
}

int load_areas(struct area *data, int lmax) {
  FILE *f = loader_get_data_file(FILE_AREAS, 0);

  char output[2048];
  jsmntok_t tokens[128];
  load_json(f, output, 2048, tokens, 128);

  int i = 1, idx = 0;
  char line[MAX_JSON_LINE_CHARS];
  while (tokens[i].end != 0 && tokens[i].end < tokens[0].end) {

    if (tokens[i].type == JSMN_STRING) {
      load_json_token(output, line, tokens, i++);
      if (strlen(line) > 0 && tokens[i].type == JSMN_STRING) {
        data[idx].id = atoi(line);
        load_json_token(output, line, tokens, i);
        strncpy(data[idx].title, line, sizeof(data[idx].title));
        idx++;
      }
    }
    if (tokens[i].end >= tokens[0].end)
      break;
    i++;
  }

  fclose(f);
  return idx;
}

int load_places(struct place *data, int lmax) {
  FILE *f = loader_get_data_file(FILE_PLACES, 1);

  char output[2048];
  jsmntok_t tokens[128];
  load_json(f, output, 2048, tokens, 128);

  int j, j_max, k, k_max, i = 1, idx = 0, trans_idx = 0;
  int in_place = 0, in_data = 1, has_title = 0;
  char line[MAX_JSON_LINE_CHARS];
  while (tokens[i].end != 0 && tokens[i].end < tokens[0].end) {
    load_json_token(output, line, tokens, i);

    if (in_place == 0 && tokens[i].type == JSMN_STRING) {
      data[idx].area_id = current_area_id;
      data[idx].id = atoi(line);
      in_place = 1;

    } else if (in_place == 1 && in_data == 0 && tokens[i].type == JSMN_OBJECT) {
      in_data = 1;
    } else if (in_data == 1 && has_title == 0 && tokens[i].type == JSMN_STRING) {
      load_json_token(output, line, tokens, ++i);
      strncpy(data[idx].title, line, sizeof(data[idx].title));

      has_title = 1;
    } else if (has_title == 1 && tokens[i].type == JSMN_ARRAY) {
      // iterate through transitions
      trans_idx = 0;
      j_max = tokens[i].size;
      for (j = 0; j < j_max; j++) {
        i++;
        if (tokens[i].type == JSMN_OBJECT) {
          // iterate thorough transition object parts
          k_max = tokens[i].size;
          i++;
          for (k = 0; k < k_max - 1; k++) {
            if (k % 2 == 0 && tokens[i].type == JSMN_STRING) {
              load_json_token(output, line, tokens, i);

              // detect transition data elements
              if (strcmp(line, "title") == 0) {
                i++; // get title value
                load_json_token(output, line, tokens, i);
                strncpy(data[idx].transitions[trans_idx].title, line,
                  sizeof(data[idx].transitions[trans_idx].title));

              } else if (strcmp(line, "target_place_id") == 0) {
                i++; // get target place id value
                load_json_token(output, line, tokens, i);
                data[idx].transitions[trans_idx].id = atoi(line);

              } else if (strcmp(line, "status") == 0) {
                i++; // get status and key item value
                load_json_token(output, line, tokens, i);
                if (strcmp(line, "locked") == 0) {
                  data[idx].transitions[trans_idx].status = TRANSITION_STATUS_LOCKED;
                } else if (strcmp(line, "closed") == 0) {
                  data[idx].transitions[trans_idx].status = TRANSITION_STATUS_CLOSED;
                } else {
                  data[idx].transitions[trans_idx].status = TRANSITION_STATUS_OPEN;
                }

              } else if (strcmp(line, "unlock_item_id") == 0) {
                i++; // get item id to unlock trans
                load_json_token(output, line, tokens, i);
                data[idx].transitions[trans_idx].ul_item_id = atoi(line);

              }
            } else {
              i++;
            }
          }
        }
        trans_idx++;
      }
      in_place = 0, in_data = 1, has_title = 0;
      idx++;
    }
    if (tokens[i].end >= tokens[0].end)
      break;
    i++;
  }

  fclose(f);
  return idx;
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
  FILE *f = loader_get_data_file(FILE_ITEMS, 1);

  char output[4096];
  jsmntok_t tokens[256];
  load_json(f, output, 4096, tokens, 256);

  int i = 1, j = 0, j_max = 0, k = 0, k_max = 0, l = 0, l_max = 0, m = 0, m_max = 0;
  int idx = 0, desc_idx = 0;
  char line[MAX_JSON_LINE_CHARS];

  if (tokens[i].type == JSMN_STRING) {
    i++;
    if (tokens[i].type == JSMN_ARRAY) {
      // iterate through items array
      j_max = tokens[i].size;
      for (j = 0; j < j_max; j++) {
        i++;
        if (tokens[i].type == JSMN_OBJECT) {
          // iterate through item object parts
          k_max = tokens[i].size;
          for (k = 0; k < k_max; k++) {
            i++;
            if (k % 2 == 0 && tokens[i].type == JSMN_STRING) {
              load_json_token(output, line, tokens, i);
              // get item object part by key
              load_json_token(output, line, tokens, i);
              if (strcmp(line, "id") == 0) {
                load_json_token(output, line, tokens, i+1);
                data[idx].id = atoi(line);
              } else if (strcmp(line, "comb_id") == 0) {
                load_json_token(output, line, tokens, i+1);
                data[idx].comb_id = atoi(line);
              } else if (strcmp(line, "comb_type") == 0) {
                load_json_token(output, line, tokens, i+1);
                if (strcmp(line, "item") == 0) {
                  data[idx].comb_type = ITEM_COMB_TYPE_ITEM;
                } else if (strcmp(line, "npc") == 0) {
                  data[idx].comb_type = ITEM_COMB_TYPE_NPC;
                } else if (strcmp(line, "trans") == 0) {
                  data[idx].comb_type = ITEM_COMB_TYPE_TRANS;
                }
              } else if (strcmp(line, "final_id") == 0) {
                load_json_token(output, line, tokens, i+1);
                data[idx].final_id = atoi(line);
              } else if (strcmp(line, "title") == 0) {
                load_json_token(output, line, tokens, i+1);
                strncpy(data[idx].title, line, sizeof(data[idx].title));
              } else if (strcmp(line, "descriptions") == 0) {
                i++;
                // todo go through descriptions array / objects
                if (tokens[i].type == JSMN_ARRAY && tokens[i].size > 0) {
                  desc_idx = 0;
                  l_max = tokens[i].size;
                  for (l = 0; l < l_max; l++) {
                    i++;
                    if (tokens[i].type == JSMN_OBJECT) {
                      load_json_token(output, line, tokens, i);
                      m_max = tokens[i].size;
                      // iterate through description object elements
                      for (m = 0; m < m_max; m++) {
                        i++;
                        if (m % 2 == 0 && tokens[i].type == JSMN_STRING) {
                          load_json_token(output, line, tokens, i);
                          if (strcmp(line, "command") == 0) {
                            load_json_token(output, line, tokens, i+1);
                            strncpy(data[idx].descriptions[desc_idx].i_command,
                              line, sizeof(data[idx].descriptions[desc_idx].i_command));
                          } else if (strcmp(line, "text") == 0) {
                            load_json_token(output, line, tokens, i+1);
                            strncpy(data[idx].descriptions[desc_idx].i_description,
                              line, sizeof(data[idx].descriptions[desc_idx].i_description));
                          }
                        }
                      }
                    }
                    desc_idx++;
                  }
                  i--; // reduce token idx after descriptions to get next object token correctly
                }
              }
            }
          }
        }
        idx++;
      }
    }
  }

  fclose(f);
  return idx;
}

/* configuration style:
 &1 // npc id
 Marunix // npc title
 1#2 // npc area and place
*/
int load_npcs(struct npc data[], int lmax) {
  int data_idx = 0, run = 1, data_type = 0;
  char npc_title[MAX_NPC_NAME_LENGTH], npc_id[24], area_id[24], place_id[24];
  FILE *f = loader_get_data_file(FILE_NPCS, 1);
  do {
    if (data_type == 0 && fscanf(f, "&%[0-9]\n", npc_id) && strlen(npc_id) > 0) {
      data[data_idx].id = atoi(npc_id);
      strncpy(npc_id, "", sizeof(npc_id));
      data_type = 1;
    } else if (data_type == 1 && fscanf(f, "%[^\n]\n", npc_title) && strlen(npc_title) > 0) {
      strncpy(data[data_idx].title, npc_title, sizeof(data[data_idx].title));
      strncpy(npc_title, "", sizeof(npc_title));
      data_type = 2;
    } else if (data_type == 2 && fscanf(f, "%[0-9]#%[0-9]\n", area_id, place_id) &&
               strlen(area_id) > 0 && strlen(place_id) > 0) {
      data[data_idx].area_id = atoi(area_id);
      data[data_idx].place_id = atoi(place_id);
      strncpy(area_id, "", sizeof(area_id));
      strncpy(place_id, "", sizeof(place_id));
      data_type = 0;
      data_idx++;
    } else {
      run = 0;
    }
  } while (run == 1);
  fclose(f);
  return data_idx;
}

int load_dialogs(struct npc npcs_data[], int nlmax, struct dialog data[], int lmax) {
  int npc_idx, title_idx, elements_count, element_idx;
  int data_mode = 0, run, ch;
  char line[1024], file_name[1024], dialog_text[DIALOG_MAX_TEXT_LENGTH], dialog_int_str[24];
  FILE *f;

  elements_count = 0;
  for (npc_idx = 0; npc_idx < nlmax; npc_idx++) {
    if (npc_idx == lmax)
      return npc_idx;

    data[npc_idx].npc_id = npcs_data[npc_idx].id;
    strncpy(line, "", 1024);
    for (title_idx = 0; npcs_data[npc_idx].title[title_idx]; title_idx++){
      line[title_idx] = tolower(npcs_data[npc_idx].title[title_idx]);
    }
    snprintf(file_name, 1024, "%s%s%s", FILE_DIALOGS_FOLDER, line, FILE_DIALOGS_POSTFIX);

    f = loader_get_data_file(file_name, 1);
    run = 1;
    element_idx = 0;
    do {
      if (data_mode == 0 && fscanf(f, "!%[0-9]\n", dialog_int_str) &&
          strlen(dialog_int_str) > 0) {
        // get dialog element id
        data[npc_idx].elements[element_idx].id = atoi(dialog_int_str);
        strncpy(dialog_int_str, "", sizeof(dialog_int_str));
        data_mode = 1;
      } else if (data_mode == 1 && fscanf(f, "%[^\n]\n", dialog_text) &&
                 strlen(dialog_text) > 0) {
        // get dialog element text
        strncpy(data[npc_idx].elements[element_idx].text, dialog_text,
          sizeof(data[npc_idx].elements[element_idx].text));
        strncpy(dialog_text, "", sizeof(dialog_text));
        data_mode = 2;
      } else if (data_mode == 2) {
        // get next ids
        int i = 0;
        while ((ch = fgetc(f)) != '\n' && fscanf(f, "%[0-9]", dialog_int_str) && dialog_int_str > 0) {
          data[npc_idx].elements[element_idx].next_mchoice = (ch == '?') ? 1 : 0;
          data[npc_idx].elements[element_idx].next_ids[i] = atoi(dialog_int_str);
          i++;
        }
        data_mode = 0;
        element_idx++;
        fseek(f, ftell(f) + 1, SEEK_SET);
      } else {
        run = 0;
      }
    } while (run == 1);
    fclose(f);
    elements_count += element_idx;
    // link dialog to npc
    npcs_data[npc_idx].c_dialog = data;
  }
  return elements_count;
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
          data[data_idx].id_trans_status[id_trans_idx] = TRANSITION_STATUS_OPEN;
          // id trans status used to get descriptions in relation to closed / locked status
          if ((ch = fgetc(f)) == '<') {
            fscanf(f, "%[^\n^/^$]", str);
            if (strcmp(str, "c") == 0) {
              data[data_idx].id_trans_status[id_trans_idx] = TRANSITION_STATUS_CLOSED;
              data[data_idx].id_trans_item_id[id_trans_idx] = 0;
            } else if (strcmp(str, "l") == 0) {
              data[data_idx].id_trans_status[id_trans_idx] = TRANSITION_STATUS_LOCKED;
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
  FILE *f = loader_get_data_file(FILE_DESCRIPTIONS, 1);
  int data_idx = 0;
  data_idx = load_descriptions_rec(f, data, data_idx);
  fclose(f);
  return ++data_idx;
}
