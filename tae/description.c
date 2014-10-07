#include <stdio.h>
#include <string.h>
#include "description.h"
#include "inventory.h"
#include "dialog.h"
#include "main.h"

struct description *descriptions;
int descriptions_count = 0;

char *help_description;

extern struct meta meta_data;
extern int current_area;
extern int current_place;

void description_set_descriptions(struct description *desc_data, int desc_data_count) {
  descriptions = desc_data;
  descriptions_count = desc_data_count;
}

void description_set_help(char *help) {
  help_description = help;
}

char *description_by_area_place() {
  // get first output with intro by using virtual description
  int desc_idx = 0, has_text = 0;
  char line[1024];
  static char output[1024];
  strncpy(output, "", sizeof(output));

  if (current_area == 0 && descriptions[desc_idx].id == 0) {
    snprintf(output, 1024, "%s\n\n", descriptions[desc_idx].text);
    current_area = 1;
    current_place = 1;
    has_text = 1;
    desc_idx++;
  }

  // get more data of the current area place
  int base_idx = -1, base_item_idx = 0, base_npc_idx = 0, base_trans_idx = 0;
  int has_item = 0, has_transition = 0, has_npc = 0;
  while (desc_idx < descriptions_count) {
    if (descriptions[desc_idx].id == current_place &&
        strlen(descriptions[desc_idx].id_verb) == 0) {

      if (base_idx == -1) {
        base_idx = desc_idx;
      } else {
        has_item = 0;
        has_transition = 0;
        has_npc = 0;

        // show place descriptions with valid item only
        for (base_item_idx = 0; base_item_idx < MAX_DESC_TEXT_ITEMS; base_item_idx++) {
          if (descriptions[base_idx].items[base_item_idx] > 0 &&
              descriptions[base_idx].items[base_item_idx] == descriptions[desc_idx].id_items[0]) {
            has_item = 1;
          }
        }
        if (has_item == 0) {
          // show place descriptions with valid npc only
          for (base_npc_idx = 0; base_npc_idx < MAX_DESC_TEXT_NPCS; base_npc_idx++) {
            if (descriptions[base_idx].npcs[base_npc_idx] > 0 &&
                descriptions[base_idx].npcs[base_npc_idx] == descriptions[desc_idx].id_npcs[0]) {
              has_npc = 1;
            }
          }
        }
        if (has_item == 0 && has_npc == 0) {
          // show place descriptions with valid transitions only
          for (base_trans_idx = 0; base_trans_idx < MAX_DESC_TEXT_TRANS; base_trans_idx++) {
            if (descriptions[base_idx].transitions[base_trans_idx] > 0 &&
                descriptions[base_idx].transitions[base_trans_idx] ==
                  descriptions[desc_idx].id_transitions[0]) {
              has_transition = 1;
            }
          }
        }
      }

      if (desc_idx == base_idx || has_item == 1 || has_transition == 1 || has_npc == 1) {
        snprintf(line, 1024, "%s ", descriptions[desc_idx].text);
        if (has_text == 1) {
          strcat(output, line);
        } else {
          snprintf(output, 1024, "%s ", descriptions[desc_idx].text);
          has_text = 1;
        }
      }
    } else if (descriptions[desc_idx].id == current_place + 1) {
      break;
    }
    desc_idx++;
  }
  strcat(output, "\n\n");

  return output;
}

char *description_by_action(struct action *caction) {
  // get action related output of the current area place
  int desc_idx = 0, i, has_text = 0, has_item = 0, has_transition = 0, has_npc = 0;
  char line[1024];
  static char output[1024];
  strncpy(output, "", sizeof(output));

  if (strlen(caction->in_command) > 0 &&
      ((caction->pitem != NULL && caction->pitem->id > 0) ||
       (caction->transition != NULL && caction->transition->id > 0) ||
       (caction->c_npc != NULL && caction->c_npc->id > 0))) {

    // for transition / item / npc action commands
    desc_idx = 0;
    while (desc_idx < descriptions_count) {
      if (descriptions[desc_idx].id == current_place &&
          strcmp(descriptions[desc_idx].id_verb, caction->in_command) == 0) {

        // check status of id item / transition / npc
        has_item = 0;
        has_transition = 0;
        has_npc = 0;
        if (caction->pitem != NULL && caction->pitem->id > 0 &&
            descriptions[desc_idx].id_items[0] == caction->pitem->id) {
          has_item = 1;

        } else if (caction->transition != NULL && caction->transition->id > 0 &&
                   descriptions[desc_idx].id_transitions[0] == caction->transition->id) {
          has_transition = 1;

        } else if (caction->c_npc != NULL && caction->c_npc->id > 0 &&
                   descriptions[desc_idx].id_npcs[0] == caction->c_npc->id) {
          has_npc = 1;
        }

        if (has_item == 1 || has_transition == 1 || has_npc == 1) {
          // check status of available text items to hide descs with non existent items
          for (i = 0; i < MAX_DESC_TEXT_ITEMS; i++) {
            struct item *citem = get_item_by_id(descriptions[desc_idx].items[i]);
            if (citem != NULL && citem->status != 0) {
              has_item = 0;
              has_transition = 0;
              has_npc = 0;
            }
          }

          if (has_item == 1 || has_transition == 1 || has_npc == 1) {
            snprintf(line, 1024, "%s ", descriptions[desc_idx].text);
            strcat(output, line);
            has_text = 1;
          }
        }

      } else if (descriptions[desc_idx].id == current_place + 1) {
        break;
      }
      desc_idx++;
    }
    if (has_text == 1)
      strcat(output, "\n\n");

    // talkto action dialog output
    if (strcmp(caction->in_command, "talkto") == 0 &&
        caction->c_npc != NULL && caction->c_npc->id > 0) {
      strcat(output, dialog_get_output());
    }

  } else if (strlen(caction->in_command) > 0) {
    // for dialog commands
    if (dialog_get_current_idx() > 0) {
      strcat(output, dialog_get_output());

    // for simple actions commands
    } else if (strcmp(caction->in_command, "description") == 0) {
      strcat(output, description_by_area_place());

    } else if (strcmp(caction->in_command, "help") == 0) {
      strcat(output, help_description);
      strcat(output, "\n");

    } else if (strcmp(caction->in_command, "inventory") == 0) {
      strcat(output, inventory_get_output(&meta_data));
    }
  }

  return output;
}