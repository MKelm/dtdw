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

  int i = 0, j, j_max;
  char line[MAX_JSON_LINE_CHARS];

  if (tokens[i].type == JSMN_ARRAY) {
    // iterate through help text lines array
    j_max = tokens[i].size;
    for (j = 0; j < j_max; j++) {
      i++;
      if (tokens[i].type == JSMN_STRING) {
        load_json_token(output, line, tokens, i);
        strcat(line, "\n");
        strcat(help, line);
      }
    }
  }
  fclose(f);
}

void load_meta(struct meta *data) {
  FILE *f = loader_get_data_file(FILE_META, 0);

  char output[2048];
  jsmntok_t tokens[128];
  load_json(f, output, 2048, tokens, 128);

  int i = 0, j, j_max;
  char line[MAX_JSON_LINE_CHARS];

  if (tokens[i].type == JSMN_OBJECT) {
    // iterate through meta object parts
    j_max = tokens[i].size;
    for (j = 0; j < j_max; j++) {
      i++;
      if (j % 2 == 0 && tokens[i].type == JSMN_STRING) {
        load_json_token(output, line, tokens, i);
        // get meta object part content by key
        if (strcmp(line, "title") == 0) {
          load_json_token(output, line, tokens, i+1);
          strncpy(data->title, line, sizeof(data->title));
        } else if (strcmp(line, "version") == 0) {
          load_json_token(output, line, tokens, i+1);
          strncpy(data->version, line, sizeof(data->version));
        } else if (strcmp(line, "author") == 0) {
          load_json_token(output, line, tokens, i+1);
          strncpy(data->author, line, sizeof(data->author));
        } else if (strcmp(line, "year") == 0) {
          load_json_token(output, line, tokens, i+1);
          data->cyear = atoi(line);
        }
      }
    }
  }
  fclose(f);
}

void load_phrases(struct phrases *data) {
  FILE *f = loader_get_data_file(FILE_PHRASES, 0);

  char output[2048];
  jsmntok_t tokens[128];
  load_json(f, output, 2048, tokens, 128);

  int i = 0, j, j_max;
  char line[MAX_JSON_LINE_CHARS];

  if (tokens[i].type == JSMN_OBJECT) {
    // iterate through phrases object parts
    j_max = tokens[i].size;
    for (j = 0; j < j_max; j++) {
      i++;
      if (j % 2 == 0 && tokens[i].type == JSMN_STRING) {
        load_json_token(output, line, tokens, i);
        // get phrase object part content by key
        if (strcmp(line, "inv_title") == 0) {
          load_json_token(output, line, tokens, i+1);
          strncpy(data->inv_title, line, sizeof(data->inv_title));
        } else if (strcmp(line, "no_inv_items") == 0) {
          load_json_token(output, line, tokens, i+1);
          strncpy(data->no_inv_items, line, sizeof(data->no_inv_items));
        } else if (strcmp(line, "items_comb") == 0) {
          load_json_token(output, line, tokens, i+1);
          strncpy(data->items_comb, line, sizeof(data->items_comb));
        } else if (strcmp(line, "items_comb_failure") == 0) {
          load_json_token(output, line, tokens, i+1);
          strncpy(data->items_comb_failure, line, sizeof(data->items_comb_failure));
        } else if (strcmp(line, "item_usage_failure") == 0) {
          load_json_token(output, line, tokens, i+1);
          strncpy(data->item_usage_failure, line, sizeof(data->item_usage_failure));
        }
      }
    }
  }
  fclose(f);
}

int load_commands(struct command *data, int lmax) {
  FILE *f = loader_get_data_file(FILE_COMMANDS, 0);

  char output[2048];
  jsmntok_t tokens[128];
  load_json(f, output, 2048, tokens, 128);

  int i = 0, j, j_max, idx = 0;
  char line[MAX_JSON_LINE_CHARS];

  if (tokens[i].type == JSMN_OBJECT) {
    // iterate through commands object parts
    j_max = tokens[i].size;
    for (j = 0; j < j_max; j++) {
      i++;
      if (j % 2 == 0 && tokens[i].type == JSMN_STRING) {
        // get command object part content
        load_json_token(output, line, tokens, i);
        strncpy(data[idx].in, line, sizeof(data[idx].in));
        load_json_token(output, line, tokens, i+1);
        strncpy(data[idx].ex, line, sizeof(data[idx].ex));
        idx++;
      }
    }
  }
  fclose(f);
  return idx;
}

int load_areas(struct area *data, int lmax) {
  FILE *f = loader_get_data_file(FILE_AREAS, 0);

  char output[2048];
  jsmntok_t tokens[128];
  load_json(f, output, 2048, tokens, 128);

  int i = 0, j, j_max, idx = 0;
  char line[MAX_JSON_LINE_CHARS];

  if (tokens[i].type == JSMN_OBJECT) {
    // iterate through areas object parts
    j_max = tokens[i].size;
    for (j = 0; j < j_max; j++) {
      i++;
      if (j % 2 == 0 && tokens[i].type == JSMN_PRIMITIVE) {
        // get area object part content
        load_json_token(output, line, tokens, i);
        data[idx].id = atoi(line);
        load_json_token(output, line, tokens, i+1);
        strncpy(data[idx].title, line, sizeof(data[idx].title));
        idx++;
      }
    }
  }
  fclose(f);
  return idx;
}

int load_places(struct place *data, int lmax) {
  FILE *f = loader_get_data_file(FILE_PLACES, 1);

  char output[2048];
  jsmntok_t tokens[128];
  load_json(f, output, 2048, tokens, 128);

  int i = 0, j, j_max, k, k_max, l, l_max, m, m_max, idx = 0, trans_idx = 0;
  char line[MAX_JSON_LINE_CHARS];

  if (tokens[i].type == JSMN_OBJECT) {
    // iterate through places parts
    j_max = tokens[i].size;
    for (j = 0; j < j_max; j++) {
      i++;
      if (tokens[i].type == JSMN_PRIMITIVE) {
        load_json_token(output, line, tokens, i);
        data[idx].area_id = current_area_id;
        data[idx].id = atoi(line);
        i++;
        if (tokens[i].type == JSMN_OBJECT && tokens[i].size > 0) {
          // iterate through place parts
          k_max = tokens[i].size;
          for (k = 0; k < k_max; k++) {
            i++;
            if (k % 2 == 0 && tokens[i].type == JSMN_STRING) {
              load_json_token(output, line, tokens, i);
              if (strcmp("title", line) == 0) {
                load_json_token(output, line, tokens, i+1);
                strncpy(data[idx].title, line, sizeof(data[idx].title));
              } else if (strcmp("transitions", line) == 0) {
                i++;
                trans_idx = 0;
                // iterrate through trans array
                l_max = tokens[i].size;
                for (l = 0; l < l_max; l++) {
                  i++;
                  // iterrate throught trans array part elements
                  m_max = tokens[i].size;
                  for (m = 0; m < m_max; m++) {
                    i++;
                    if (m % 2 == 0 && tokens[i].type == JSMN_STRING) {
                      // get trans array part by key
                      load_json_token(output, line, tokens, i);
                      if (strcmp("title", line) == 0) {
                        load_json_token(output, line, tokens, i+1);
                        strncpy(data[idx].transitions[trans_idx].title, line,
                          sizeof(data[idx].transitions[trans_idx].title));
                      } else if (strcmp("target_place_id", line) == 0) {
                        load_json_token(output, line, tokens, i+1);
                        data[idx].transitions[trans_idx].id = atoi(line);
                      } else if (strcmp("status", line) == 0) {
                        load_json_token(output, line, tokens, i+1);
                        // get status and key item value
                        if (strcmp(line, "locked") == 0) {
                          data[idx].transitions[trans_idx].status = TRANSITION_STATUS_LOCKED;
                        } else if (strcmp(line, "closed") == 0) {
                          data[idx].transitions[trans_idx].status = TRANSITION_STATUS_CLOSED;
                        } else {
                          data[idx].transitions[trans_idx].status = TRANSITION_STATUS_OPEN;
                        }
                      } else if (strcmp("unlock_item_id", line) == 0) {
                        load_json_token(output, line, tokens, i+1);
                        data[idx].transitions[trans_idx].ul_item_id = atoi(line);
                      }
                    }
                  }
                  trans_idx++;
                }
                i--;
              }
            }
          }
        }
      }
      idx++;
    }
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

  int i = 0, j, j_max, k, k_max, l, l_max, m, m_max;
  int idx = 0, desc_idx = 0;
  char line[MAX_JSON_LINE_CHARS];

  if (tokens[i].type == JSMN_OBJECT) {
    // iterate through items objects
    j_max = tokens[i].size;
    for (j = 0; j < j_max; j++) {
      i++;
      if (j % 2 == 0 && tokens[i].type == JSMN_PRIMITIVE) {
        load_json_token(output, line, tokens, i);
        data[idx].id = atoi(line);

        i++;
        if (tokens[i].type == JSMN_OBJECT) {
          // iterate through item part object elements
          k_max = tokens[i].size;
          for (k = 0; k < k_max; k++) {
            i++;
            if (j % 2 == 0 && tokens[i].type == JSMN_STRING) {
              // get item part object element content by key
              load_json_token(output, line, tokens, i);
              if (strcmp("title", line) == 0) {
                load_json_token(output, line, tokens, i+1);
                strncpy(data[idx].title, line, sizeof(data[idx].title));

              } else if (strcmp("comb_id", line) == 0) {
                load_json_token(output, line, tokens, i+1);
                data[idx].comb_id = atoi(line);

              } else if (strcmp("comb_type", line) == 0) {
                load_json_token(output, line, tokens, i+1);
                if (strcmp(line, "item") == 0) {
                  data[idx].comb_type = ITEM_COMB_TYPE_ITEM;
                } else if (strcmp(line, "npc") == 0) {
                  data[idx].comb_type = ITEM_COMB_TYPE_NPC;
                } else if (strcmp(line, "trans") == 0) {
                  data[idx].comb_type = ITEM_COMB_TYPE_TRANS;
                }

              } else if (strcmp("final_id", line) == 0) {
                load_json_token(output, line, tokens, i+1);
                data[idx].final_id = atoi(line);

              } else if (strcmp("descriptions", line) == 0) {
                i++;
                // go through descriptions array / objects
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
                }
                i--;
              }
            }
          }
          idx++;
        }
        i--;
      }
    }
  }
  fclose(f);
  return idx;
}

int load_npcs(struct npc data[], int lmax) {
  FILE *f = loader_get_data_file(FILE_NPCS, 1);

  char output[2048];
  jsmntok_t tokens[128];
  load_json(f, output, 2048, tokens, 128);

  int i = 0, j, j_max, k, k_max, idx = 0;
  char line[MAX_JSON_LINE_CHARS];
  if (tokens[i].type == JSMN_OBJECT) {
    j_max = tokens[i].size;
    for (j = 0; j < j_max; j++) {
      i++;
      if (j % 2 == 0 && tokens[i].type == JSMN_PRIMITIVE) {
        load_json_token(output, line, tokens, i);
        data[idx].id = atoi(line);

        i++;
        // iterate through item part object elements
        k_max = tokens[i].size;
        for (k = 0; k < k_max; k++) {
          i++;
          if (j % 2 == 0 && tokens[i].type == JSMN_STRING) {
            // get npc object part by key
            load_json_token(output, line, tokens, i);
            if (strcmp(line, "name") == 0) {
              load_json_token(output, line, tokens, i+1);
              strncpy(data[idx].title, line, sizeof(data[idx].title));

            } else if (strcmp(line, "area_id") == 0) {
              load_json_token(output, line, tokens, i+1);
              data[idx].area_id = atoi(line);

            } else if (strcmp(line, "place_id") == 0) {
              load_json_token(output, line, tokens, i+1);
              data[idx].place_id = atoi(line);
            }
          }
        }
        i--;
        idx++;
      }
    }
  }
  fclose(f);
  return idx;
}

int load_dialogs(struct npc npcs_data[], int nlmax, struct dialog data[], int lmax) {
  int npc_idx, title_idx, elements_count;
  char file_name[1024], line[MAX_JSON_LINE_CHARS];
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

    char output[2048];
    jsmntok_t tokens[128];
    load_json(f, output, 2048, tokens, 128);

    int i = 0, j, j_max, k, k_max, l, l_max, element_idx = 0;
    if (tokens[i].type == JSMN_OBJECT) {
      j_max = tokens[i].size;
      for (j = 0; j < j_max; j++) {
        i++;
        if (j % 2 == 0 && tokens[i].type == JSMN_PRIMITIVE) {
          load_json_token(output, line, tokens, i);
          data[npc_idx].elements[element_idx].id = atoi(line);
          printf("dialog id %s\n", line);

          i++;
          // iterate through item part object elements
          k_max = tokens[i].size;
          for (k = 0; k < k_max; k++) {
            i++;
            if (j % 2 == 0 && tokens[i].type == JSMN_STRING) {
              // get npc object part by key
              load_json_token(output, line, tokens, i);
              if (strcmp(line, "text") == 0) {
                load_json_token(output, line, tokens, i+1);
                strncpy(data[npc_idx].elements[element_idx].text,
                  line, sizeof(data[npc_idx].elements[element_idx].text));

              } else if (strcmp(line, "is_multi_choice") == 0) {
                load_json_token(output, line, tokens, i+1);
                data[npc_idx].elements[element_idx].next_mchoice = atoi(line);

              } else if (strcmp(line, "next_ids") == 0) {

                if (tokens[i+1].type == JSMN_ARRAY && tokens[i+1].size > 0) {
                  i++;
                  // iterate through next ids array
                  l_max = tokens[i].size;
                  for (l = 0; l < l_max; l++) {
                    i++;
                    if (tokens[i].type == JSMN_PRIMITIVE) {
                      load_json_token(output, line, tokens, i);
                      data[npc_idx].elements[element_idx].next_ids[l] = atoi(line);
                       printf("dialog next id %s\n", line);
                    }
                  }
                  i--;
                }

              }
            }
          }
          i--;
          element_idx++;
        }
      }
    }

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
