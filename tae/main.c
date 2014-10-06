#include <stdio.h>
#include <string.h>
#include <locale.h>
#include "main.h"
#include "loader.h"
#include "inventory.h"
#include "display.h"

int current_area = 0;
int current_place = 0;

char help_text[2048];

// dc: 0 == commands, 2 == areas, 3 == places, 4 == items,
//     5 == descriptions, 6 = transitions
int data_counts[6];
struct meta meta_data;
struct command commands_data[MAX_COMMANDS];
struct area areas_data[MAX_AREAS];
struct place places_data[MAX_PLACES];
struct item items_data[MAX_ITEMS];
struct description descriptions_data[MAX_DESCRIPTIONS];
struct placetrans transitions_data[MAX_PLACETRANS];

int main(void) {
  setlocale (LC_ALL, "");

  load_data();

  dsp_init();

  dsp_windows_init();
  dsp_set_meta(&meta_data);

  int location_change = 1, output_change = 0, quit = 0;
  int *area_place_idx;
  char *input;

  struct action caction;
  init_action(&caction);

  do {
    // check location change, change header display output if needed
    if (location_change == 1) {
      dsp_set_output(desc_get_output()); // output location description
      area_place_idx = get_area_place_idx();
      dsp_set_location(
        &areas_data[area_place_idx[0]], &places_data[area_place_idx[1]]
      );
      location_change = 0;
    }
    // check output change, change output display window if needed
    if (output_change == 1) {
      dsp_set_output(action_get_output(&caction));
      output_change = 0;
      init_action(&caction);
    }
    // input prompt
    input = dsp_get_input();
    // check input to set action flags
    if (strlen(input) > 0) {
      caction = get_input_action(input);
      if (strlen(caction.in_command) > 0 &&
          strcasecmp(caction.in_command, "quit") != 0) {

        if (strcmp(caction.in_command, "use") == 0 &&
            caction.transition != NULL && caction.transition->id > 0) {
          // output transition action description before location change
          dsp_set_output(action_get_output(&caction));
          current_place = caction.transition->id;
          location_change = 1;

        } else if (strcmp(caction.in_command, "pickup") == 0 &&
                   (caction.pitem != NULL && caction.pitem->id > 0)) {
          // simple implementation to get inventory item
          // todo: extended logic with definitions to handle item / description status
          inventory_add_item(caction.pitem);
        }
        output_change = 1;
      } else if (strcasecmp(caction.in_command, "quit") == 0) {
        quit = 1;
      }
    }
  } while (quit == 0);

  dsp_end();
  return 0;
}

void init_action(struct action *caction) {
  strcpy(caction->in_command, "");
  caction->transition = NULL;
  caction->pitem = NULL;
  caction->sitem = NULL;
}

struct action get_input_action(char *input) {
  static struct action iaction;
  init_action(&iaction);

  char inputarr[4][24], commandarr[2][24];
  char *ptr;
  ptr = strtok(input, " ");
  int i = 0;
  while (ptr != NULL) {
    if (i < 4) {
      strcpy(inputarr[i], ptr);
      ptr = strtok(NULL, " ");
      i++;
    }
  }
  if (i == 1 || i == 2 || i == 4) {
    char *icommand; // internal command
    char ccommand[24];
    if (i == 1) {
      // simple command
      icommand = get_internal_command(inputarr[0]);
    } else if (i == 2) {
      // transition/item action command
      snprintf(ccommand, 24, "%s ??", inputarr[0]);
      icommand = get_internal_command(ccommand);
    } else if (i == 4) {
      // item combinition action command
      snprintf(ccommand, 24, "%s ?? %s ??", inputarr[0], inputarr[2]);
      icommand = get_internal_command(ccommand);
    }
    if (strlen(icommand) > 0) {
      if (i > 1) {
        // for item/transition or combination actions
        ptr = strtok(icommand, " ");
        int j = 0;
        while (ptr != NULL) {
          if (j < 4) {
            strcpy(commandarr[j], ptr);
            ptr = strtok(NULL, " ");
            j++;
          }
        }
        if (j > 0) {
          if (i == 2) {
            // more actions for item/transition action command
            strcpy(iaction.in_command, commandarr[0]);
            iaction.pitem = get_item(inputarr[1]);
            if (iaction.pitem == NULL) {
              iaction.transition = get_transition(inputarr[1]);
            }
            return iaction;
          } else if (i == 4) {
            // more actions for item combinition action command
            strcpy(iaction.in_command, commandarr[0]);
            iaction.pitem = get_item(inputarr[1]);
            iaction.sitem = get_item(inputarr[3]);
            return iaction;
          }
        }
      } else if (i == 1) {
        // for simple actions
        strcpy(iaction.in_command, icommand);
      }
    }
  }
  return iaction;
}

struct item *get_item(char *title) {
  int i;
  for (i = 0; i < data_counts[4]; i++) {
    if (strcasecmp(items_data[i].title, title) == 0) {
      return &items_data[i];
    }
  }
  return NULL;
}

struct item *get_item_by_id(int id) {
  int i;
  for (i = 0; i < data_counts[4]; i++) {
    if (items_data[i].id == id) {
      return &items_data[i];
    }
  }
  return NULL;
}

struct placetrans *get_transition(char *ttransition) {
  int i;
  for (i = 0; i < data_counts[5]; i++) {
    if (strcasecmp(transitions_data[i].title, ttransition) == 0) {
      return &transitions_data[i];
    }
  }
  return NULL;
}

char *get_internal_command(char *ccommand) {
  int i;
  for (i = 0; i < data_counts[0]; i++) {
    if (strcmp(commands_data[i].ex, ccommand) == 0) {
      return commands_data[i].in;
    }
  }
  return ccommand;
}

int *get_area_place_idx(void) {
  static int area_place_idx[2];
  int areaidx = 0, placeidx = 0;

  int i = 0, run = 1;
  while (run == 1) {
    if (areas_data[i].id == current_area) {
      areaidx = i;
      run = 0;
    } else
     i++;
  }
  i = 0, run = 1;
  while (run == 1) {
    if (places_data[i].area_id == current_area &&
        places_data[i].id == current_place) {
      placeidx = i;
      run = 0;
    } else
     i++;
  }

  area_place_idx[0] = areaidx;
  area_place_idx[1] = placeidx;
  return area_place_idx;
}

char *action_get_output(struct action *caction) {
  // get action related output of the current area place
  int desc_idx = 0, i, has_text = 0, has_item, has_transition;
  char line[1024];
  static char output[1024];
  strncpy(output, "", sizeof(output));

  if (strlen(caction->in_command) > 0 &&
      ((caction->pitem != NULL && caction->pitem->id > 0) ||
       (caction->transition != NULL && caction->transition->id > 0))) {
    // for transition / item action commands
    desc_idx = 0;
    while (desc_idx < data_counts[4]) {
      if (descriptions_data[desc_idx].id == current_place &&
          strcmp(descriptions_data[desc_idx].id_verb, caction->in_command) == 0) {

        // check status of id item / transition
        has_item = 0;
        has_transition = 0;
        if (caction->pitem != NULL && caction->pitem->id > 0 &&
            descriptions_data[desc_idx].id_items[0] == caction->pitem->id) {
          has_item = 1;

        } else if (caction->transition != NULL && caction->transition->id > 0 &&
                   descriptions_data[desc_idx].id_transitions[0] == caction->transition->id) {
          has_transition = 1;
        }

        if (has_item == 1 || has_transition == 1) {
          // check status of available text items to hide descs with non existent items
          for (i = 0; i < MAX_DESC_TEXT_ITEMS; i++) {
            struct item *citem = get_item_by_id(descriptions_data[desc_idx].items[i]);
            if (citem->status != 0) {
              has_item = 0;
              has_transition = 0;
            }
          }

          if (has_item == 1 || has_transition == 1) {
            snprintf(line, 1024, "%s ", descriptions_data[desc_idx].text);
            strcat(output, line);
            has_text = 1;
          }
        }

      } else if (descriptions_data[desc_idx].id == current_place + 1) {
        break;
      }
      desc_idx++;
    }
    if (has_text == 1)
      strcat(output, "\n\n");

  } else if (strlen(caction->in_command) > 0) {
    // for simple actions commands
    if (strcmp(caction->in_command, "description") == 0) {
      strcat(output, desc_get_output());

    } else if (strcmp(caction->in_command, "help") == 0) {
      strcat(output, help_text);
      strcat(output, "\n");

    } else if (strcmp(caction->in_command, "inventory") == 0) {
      strcat(output, inventory_get_output(&meta_data));
    }
  }

  return output;
}

char *desc_get_output() {
  // get first output with intro by using virtual description
  int desc_idx = 0, has_text = 0;
  char line[1024];
  static char output[1024];
  strncpy(output, "", sizeof(output));

  if (current_area == 0 && descriptions_data[desc_idx].id == 0) {
    snprintf(output, 1024, "%s\n\n", descriptions_data[desc_idx].text);
    current_area = 1;
    current_place = 1;
    has_text = 1;
    desc_idx++;
  }

  // get more data of the current area place
  while (desc_idx < data_counts[4]) {
    if (descriptions_data[desc_idx].id == current_place &&
        strlen(descriptions_data[desc_idx].id_verb) == 0) {

      snprintf(line, 1024, "%s ", descriptions_data[desc_idx].text);
      if (has_text == 1) {
        strcat(output, line);
      } else {
        snprintf(output, 1024, "%s ", descriptions_data[desc_idx].text);
        has_text = 1;
      }
    } else if (descriptions_data[desc_idx].id == current_place + 1) {
      break;
    }
    desc_idx++;
  }
  strcat(output, "\n\n");

  return output;
}

void load_data(void) {
  // help text
  load_help(help_text);
  // meta
  load_meta(&meta_data);
  // commands
  data_counts[0] = load_commands(commands_data, MAX_COMMANDS);
  // areas
  data_counts[1] = load_areas(areas_data, MAX_AREAS);
  // places
  data_counts[2] = load_places(places_data, MAX_PLACES);
  // items
  data_counts[3] = load_items(items_data, MAX_ITEMS);
  // descriptions
  data_counts[4] = load_descriptions(descriptions_data, MAX_DESCRIPTIONS);
  // transitions
  data_counts[5] = load_transitions(
    transitions_data, MAX_PLACETRANS, places_data, data_counts[2], MAX_PLACE_TRANSITIONS
  );
}
