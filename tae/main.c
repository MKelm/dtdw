#include <stdio.h>
#include <string.h>
#include <locale.h>
#include "main.h"
#include "loader.h"
#include "description.h"
#include "inventory.h"
#include "dialog.h"
#include "display.h"

int current_area = 0;
int current_place = 0;

// dc: 0 == commands, 1 == areas, 2 == places, 3 == items,
//     4 == descriptions, 5 = transitions, 6 = npcs, 7 = dialogs
int data_counts[8];
struct meta meta_data;
struct command commands_data[MAX_COMMANDS];
struct area areas_data[MAX_AREAS];
struct place places_data[MAX_PLACES];
struct item items_data[MAX_ITEMS];
struct description descriptions_data[MAX_DESCRIPTIONS];
struct placetrans transitions_data[MAX_PLACETRANS];
struct npc npcs_data[MAX_NPCS];
struct dialog dialogs_data[MAX_DIALOGS];

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
      dsp_set_output(description_by_area_place()); // output area place description
      area_place_idx = get_area_place_idx();
      dsp_set_location(
        &areas_data[area_place_idx[0]], &places_data[area_place_idx[1]]
      );
      location_change = 0;
    }
    // check output change, change output display window if needed
    if (output_change == 1) {
      dsp_set_output(description_by_action(&caction));
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

        output_change = 1;
        if (dialog_get_current_idx() > -1) {
          // dialog mode
          output_change = 0;
          if (strcmp(caction.in_command, "close") == 0) {
            dialog_close();
            init_action(&caction);
          } else {
            // handle input for dialog
            output_change = dialog_handle_input(caction.in_command);
          }

        } if (strcmp(caction.in_command, "use") == 0 &&
            caction.transition != NULL && caction.transition->id > 0) {
          // output transition action description before location change
          dsp_set_output(description_by_action(&caction));
          current_place = caction.transition->id;
          location_change = 1;

        } else if (strcmp(caction.in_command, "pickup") == 0 &&
                   caction.pitem != NULL && caction.pitem->id > 0) {
          // get inventory item if it exists at the current place
          if (caction.pitem->status == 0) {
            inventory_add_item(caction.pitem);
          } else {
            // no valid item selection / output
            output_change = 0;
            init_action(&caction);
          }

        } else if (strcmp(caction.in_command, "talkto") == 0 &&
                   caction.c_npc != NULL && caction.c_npc->id > 0) {

          dialog_set_current(caction.c_npc->c_dialog);
        }

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
  caction->c_npc = NULL;
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

            if (iaction.pitem == NULL && iaction.transition == NULL) {
              iaction.c_npc = get_npc(inputarr[1]);
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

struct placetrans *get_transition(char *title) {
  int i;
  for (i = 0; i < data_counts[5]; i++) {
    if (strcasecmp(transitions_data[i].title, title) == 0) {
      return &transitions_data[i];
    }
  }
  return NULL;
}

struct item *get_item(char *title) {
  int i;
  for (i = 0; i < data_counts[3]; i++) {
    if (strcasecmp(items_data[i].title, title) == 0) {
      return &items_data[i];
    }
  }
  return NULL;
}

struct item *get_item_by_id(int id) {
  int i;
  for (i = 0; i < data_counts[3]; i++) {
    if (items_data[i].id == id) {
      return &items_data[i];
    }
  }
  return NULL;
}

struct npc *get_npc(char *title) {
  int i;
  for (i = 0; i < data_counts[6]; i++) {
    if (strcasecmp(npcs_data[i].title, title) == 0) {
      return &npcs_data[i];
    }
  }
  return NULL;
}

void load_data(void) {
  // help text
  char help_text[2048];
  load_help(help_text);
  description_set_help(help_text);
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
  description_set_descriptions(descriptions_data, data_counts[4]);

  // transitions related to places
  data_counts[5] = load_transitions(
    transitions_data, MAX_PLACETRANS, places_data, data_counts[2], MAX_PLACE_TRANSITIONS
  );
  // npcs
  data_counts[6] = load_npcs(npcs_data, MAX_NPCS);
  // dialogs related to npcs
  data_counts[7] = load_dialogs(
    npcs_data, data_counts[6], dialogs_data, MAX_DIALOGS
  );
}
