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

FILE *loader_get_data_file(char *file_name, short in_area, short place_id) {
  char tmp1[256] = "", tmp2[256] = "", tmp3[256] = "";
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
  if (place_id > 0) {
    strcat(tmp2, "p");
    if (place_id < 10) {
      strcat(tmp2, "00");
    } else if (place_id < 100) {
      strcat(tmp2, "0");
    }
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "%d/", place_id);
    strcat(tmp2, buffer);
  }
  strcat(tmp1, tmp2);
  strncpy(tmp3, file_name, 256);
  strcat(tmp1, tmp3);
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

void load_full_text(FILE *f, char *text) {
  char output[MAX_HELP_TEXT_CHARS];
  jsmntok_t tokens[128];
  load_json(f, output, 2048, tokens, 128);

  int i = 0, j, j_max;
  char line[MAX_JSON_LINE_CHARS];

  if (tokens[i].type == JSMN_ARRAY) {
    // iterate through full text lines array
    j_max = tokens[i].size;
    for (j = 0; j < j_max; j++) {
      i++;
      if (tokens[i].type == JSMN_STRING) {
        load_json_token(output, line, tokens, i);
        strcat(line, "\n");
        strcat(text, line);
      }
    }
  }
}

void load_help(char *help) {
  FILE *f = loader_get_data_file(FILE_HELP, 0, 0);
  load_full_text(f, help);
  fclose(f);
}

void load_meta(struct meta *data) {
  FILE *f = loader_get_data_file(FILE_META, 0, 0);

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
  FILE *f = loader_get_data_file(FILE_PHRASES, 0, 0);

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
  FILE *f = loader_get_data_file(FILE_COMMANDS, 0, 0);

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
  FILE *f = loader_get_data_file(FILE_AREAS, 0, 0);

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
      if (j % 2 == 0 && tokens[i].type == JSMN_STRING) {
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
  FILE *f = loader_get_data_file(FILE_PLACES, 1, 0);

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
                          data[idx].transitions[trans_idx].status = STATUS_TRANSITION_LOCKED;
                        } else if (strcmp(line, "closed") == 0) {
                          data[idx].transitions[trans_idx].status = STATUS_TRANSITION_CLOSED;
                        } else {
                          data[idx].transitions[trans_idx].status = STATUS_TRANSITION_OPEN;
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
        idx++;
      }
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
  FILE *f = loader_get_data_file(FILE_ITEMS, 1, 0);

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
  FILE *f = loader_get_data_file(FILE_NPCS, 1, 0);

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

    f = loader_get_data_file(file_name, 1, 0);

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

void load_intro(char *intro) {
  FILE *f = loader_get_data_file(FILE_INTRO, 0, 0);
  load_full_text(f, intro);
  fclose(f);
}

void load_desc_text_ids(char *output, jsmntok_t *tokens, int *i,
                        int *data_idx, struct description *data, char id_type) {
  int j, j_max, id_idx;
  char line[MAX_JSON_LINE_CHARS];
  *i = *i + 1;
  if (tokens[*i].type == JSMN_ARRAY && tokens[*i].size > 0) {
    j_max = tokens[*i].size;
    id_idx = 0;
    // iterate through elements of text ids array
    for (j = 0; j < j_max; j++) {
      *i = *i + 1;
      if (tokens[*i].type == JSMN_PRIMITIVE) {
        load_json_token(output, line, tokens, *i);
        switch (id_type) {
          case 0:
            // get transition id(s)
            data[*data_idx].transition_ids[id_idx] = atoi(line);
            break;
          case 1:
            // get item id(s)
            data[*data_idx].item_ids[id_idx] = atoi(line);
            break;
          case 2:
            // get npc id(s)
            data[*data_idx].npc_ids[id_idx] = atoi(line);
            break;
        }
        id_idx++;
      }
    }
  }
  *i = *i - 1;
}

void load_desc_cond_element(char *output, jsmntok_t *tokens, int *i,
                            int *data_idx, struct description *data, int elem_type) {
  int j, j_max;
  char line[MAX_JSON_LINE_CHARS];
  *i = *i + 1;
  if (tokens[*i].type == JSMN_OBJECT && tokens[*i].size > 0) {
    j_max = tokens[*i].size;
    // iterate through sub-elements of condition element
    for (j = 0; j < j_max; j++) {
      *i = *i + 1;
      // get each condition sub-element key to perform further actions
      if (j % 2 == 0 && tokens[*i].type == JSMN_STRING) {
        load_json_token(output, line, tokens, *i);
        if (strcmp(line, "id") == 0) {
          data[*data_idx].cond.elem_type = elem_type;
          load_json_token(output, line, tokens, *i + 1);
          switch (elem_type) {
            case DESC_ELEM_TYPE_TRANSITION: // transition
              data[*data_idx].cond.elem_id = atoi(line);
              data[*data_idx].cond.elem_status = STATUS_TRANSITION_OPEN;
              break;
            case DESC_ELEM_TYPE_ITEM: // item
              data[*data_idx].cond.elem_id = atoi(line);
              data[*data_idx].cond.elem_status = STATUS_ITEM_NORMAL;
              break;
            case DESC_ELEM_TYPE_NPC: // npc
              data[*data_idx].cond.elem_id = atoi(line);
              break;
          }
        } else if (strcmp(line, "status") == 0) {
          // status for condition transition / item
          load_json_token(output, line, tokens, *i + 1);
          if (strcmp(line, "locked") == 0) {
            data[*data_idx].cond.elem_status = STATUS_TRANSITION_LOCKED;
          } else if (strcmp(line, "closed") == 0) {
            data[*data_idx].cond.elem_status = STATUS_TRANSITION_CLOSED;
          } else if (strcmp(line, "pushed") == 0) {
            data[*data_idx].cond.elem_status = STATUS_ITEM_PUSHED;
          } else if (strcmp(line, "pulled") == 0) {
            data[*data_idx].cond.elem_status = STATUS_ITEM_PULLED;
          }
        }
      }
    }
  }
  *i = *i - 1;
}

void load_desc_cond_action(char *output, jsmntok_t *tokens, int *i,
                           int *data_idx, struct description *data) {
  int j, j_max;
  char line[MAX_JSON_LINE_CHARS];
  *i = *i + 1;
  if (tokens[*i].type == JSMN_OBJECT && tokens[*i].size > 0) {
    j_max = tokens[*i].size;
    // iterate through sub-elements of condition action element
    for (j = 0; j < j_max; j++) {
      *i = *i + 1;
      // get each condition sub-element key to perform further actions
      if (j % 2 == 0 && tokens[*i].type == JSMN_STRING) {
        load_json_token(output, line, tokens, *i);
        if (strcmp(line, "command") == 0) {
          // load command sub-element
          load_json_token(output, line, tokens, *i + 1);
          strncpy(data[*data_idx].cond.action_command, line,
            sizeof(data[*data_idx].cond.action_command));
        } else if (strcmp(line, "item_id") == 0) {
          // load item_id sub-element
          load_json_token(output, line, tokens, *i + 1);
          data[*data_idx].cond.action_item_id = atoi(line);
        }
      }
    }
  }
  *i = *i - 1;
}

void load_desc_condition(char *output, jsmntok_t *tokens, int *i,
                         int *data_idx, struct description *data) {
  int j, j_max;
  char line[MAX_JSON_LINE_CHARS];
  *i = *i + 1;
  if (tokens[*i].type == JSMN_OBJECT && tokens[*i].size > 0) {
    j_max = tokens[*i].size;
    // iterate through elements of condition
    for (j = 0; j < j_max; j++) {
      *i = *i + 1;
      // get each condition element key to perform further actions
      if (j % 2 == 0 && tokens[*i].type == JSMN_STRING) {
        load_json_token(output, line, tokens, *i);
        if (strcmp(line, "transition") == 0) {
          // load description condition element transition
          load_desc_cond_element(
            output, tokens, i, data_idx, data, DESC_ELEM_TYPE_TRANSITION
          );
        } else if (strcmp(line, "item") == 0) {
          // load description condition element item
          load_desc_cond_element(
            output, tokens, i, data_idx, data, DESC_ELEM_TYPE_ITEM
          );
        } else if (strcmp(line, "npc") == 0) {
          // load description condition element npc
          load_desc_cond_element(
            output, tokens, i, data_idx, data, DESC_ELEM_TYPE_NPC
          );
        } else if (strcmp(line, "action") == 0) {
          // load description condition element action
          load_desc_cond_action(output, tokens, i, data_idx, data);
        }
      }
    }
  }
  *i = *i - 1;
}

void load_desc_element(char *output, jsmntok_t *tokens, int *i,
                       int *data_idx, struct description *data) {
  int j, j_max;
  char line[MAX_JSON_LINE_CHARS];
  if (tokens[*i].type == JSMN_OBJECT && tokens[*i].size > 0) {
    j_max = tokens[*i].size;
    // iterate through elements of description
    for (j = 0; j < j_max; j++) {
      *i = *i + 1;
      // get each element key to perform further actions
      if (j % 2 == 0 && tokens[*i].type == JSMN_STRING) {
        load_json_token(output, line, tokens, *i);

        if (strcmp(line, "condition") == 0) {
          // load condition
          load_desc_condition(output, tokens, i, data_idx, data);
        } else if (strcmp(line, "text") == 0) {
          // load text
          load_json_token(output, line, tokens, *i + 1);
          strncpy(data[*data_idx].text, line, sizeof(data[*data_idx].text));
        } else if (strcmp(line, "transitions") == 0) {
          // load transitions
          load_desc_text_ids(
            output, tokens, i, data_idx, data, DESC_ELEM_TYPE_TRANSITION
          );
        } else if (strcmp(line, "items") == 0) {
          // load items
          load_desc_text_ids(
            output, tokens, i, data_idx, data, DESC_ELEM_TYPE_ITEM
          );
        } else if (strcmp(line, "npcs") == 0) {
          // load npcs
          load_desc_text_ids(
            output, tokens, i, data_idx, data, DESC_ELEM_TYPE_NPC
          );
        }
      }
    }
  }
}

int load_descriptions(struct description *data, int lmax,
                      struct place *places_data, int places_lmax) {
  int place_idx, data_idx, i, j, j_max;
  data_idx = 0;
  for (place_idx = 0; place_idx < places_lmax; place_idx++) {
    FILE *f = loader_get_data_file(FILE_DESCRIPTIONS, 1, places_data[place_idx].id);

    char output[6000];
    jsmntok_t tokens[1024];
    load_json(f, output, 6000, tokens, 1024);

    i = 0;
    if (tokens[i].type == JSMN_ARRAY && tokens[i].size > 0) {
      j_max = tokens[i].size;
      // iterate through array of descriptions
      for (j = 0; j < j_max; j++) {
        i++;
        // set default condition, place id
        data[data_idx].cond.place_id = places_data[place_idx].id;
        // load description element with condition, text and text elements
        load_desc_element(output, tokens, &i, &data_idx, data);
        data_idx = data_idx + 1;
      }
    }
    fclose(f);
  }

  return data_idx;
}
