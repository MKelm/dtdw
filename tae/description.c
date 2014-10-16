#include <stdio.h>
#include <string.h>
#include "description.h"
#include "inventory.h"
#include "dialog.h"
#include "main.h"

struct description *descriptions;
int descriptions_count = 0;

char *help_text, *intro_text;

extern struct phrases phrases_data;
extern int current_area;
extern int current_place;

void description_set_descriptions(struct description *desc_data, int desc_data_count) {
  descriptions = desc_data;
  descriptions_count = desc_data_count;
}

void description_set_intro(char *intro) {
  intro_text = intro;
}

void description_set_help(char *help) {
  help_text = help;
}

char *description_by_area_place() {
  // get first output with intro by using virtual description
  int desc_idx = 0, has_text = 0;
  char line[1024];
  static char output[1024];
  strncpy(output, "", sizeof(output));

  if (current_area == 0 && current_place == 0) {
    snprintf(output, 1024, "%s\n", intro_text);
    current_area = 1;
    current_place = 1;
    has_text = 1;
  }

  // get more data of the current area place
  int base_idx = -1, base_item_idx = 0, base_npc_idx = 0, base_trans_idx = 0;
  int has_item = 0, has_transition = 0, has_npc = 0;
  while (desc_idx < descriptions_count) {
    if (descriptions[desc_idx].cond.place_id == current_place &&
        strlen(descriptions[desc_idx].cond.action_command) == 0) {

      if (base_idx == -1) {
        base_idx = desc_idx;
      } else {
        has_item = 0;
        has_transition = 0;
        has_npc = 0;

        // show place descriptions with valid item only
        for (base_item_idx = 0; base_item_idx < MAX_DESC_TEXT_ITEMS; base_item_idx++) {
          if (descriptions[base_idx].item_ids[base_item_idx] > 0 &&
              descriptions[desc_idx].cond.elem_id > 0 &&
              descriptions[desc_idx].cond.elem_type == DESC_ELEM_TYPE_ITEM &&
              descriptions[base_idx].item_ids[base_item_idx] == descriptions[desc_idx].cond.elem_id) {

            struct item *citem = get_item_by_id(descriptions[desc_idx].cond.elem_id);
            if (citem->status.current == descriptions[desc_idx].cond.elem_status) {
              has_item = 1;
            }
          }
        }
        if (has_item == 0) {
          // show place descriptions with valid npc only
          for (base_npc_idx = 0; base_npc_idx < MAX_DESC_TEXT_NPCS; base_npc_idx++) {
            if (descriptions[base_idx].npc_ids[base_npc_idx] > 0 &&
                descriptions[desc_idx].cond.elem_id > 0 &&
                descriptions[desc_idx].cond.elem_type == DESC_ELEM_TYPE_NPC &&
                descriptions[base_idx].npc_ids[base_npc_idx] == descriptions[desc_idx].cond.elem_id) {
              has_npc = 1;
            }
          }
        }
        if (has_item == 0 && has_npc == 0) {
          // show place descriptions with valid transitions only
          for (base_trans_idx = 0; base_trans_idx < MAX_DESC_TEXT_TRANS; base_trans_idx++) {
            if (descriptions[base_idx].transition_ids[base_trans_idx] > 0 &&
                descriptions[desc_idx].cond.elem_id > 0 &&
                descriptions[desc_idx].cond.elem_type == DESC_ELEM_TYPE_TRANSITION &&
                descriptions[base_idx].transition_ids[base_trans_idx] ==
                  descriptions[desc_idx].cond.elem_id) {
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
    } else if (descriptions[desc_idx].cond.place_id == current_place + 1) {
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

  if (strlen(caction->in_command) == 0) {
    // no active command
    return output;
  }

  if (caction->p_item != NULL && caction->s_item != NULL) {

    // item combination actions
    if (caction->f_item == NULL) {
      snprintf(line, 1024, phrases_data.items_comb_failure, "");
    } else {
      snprintf(line, 1024, phrases_data.items_comb,
        caction->p_item->title, caction->s_item->title, caction->f_item->title);
    }
    strcat(line, "\n\n");
    strcat(output, line);
    has_text = 1;

  } else if ((caction->p_item != NULL && caction->p_item->id > 0) ||
             (caction->transition != NULL && caction->transition->target_place_id > 0) ||
             (caction->c_npc != NULL && caction->c_npc->id > 0)) {

    // for inventory item actions (lookat / give to)
    if (caction->p_item != NULL && caction->p_item->id > 0 &&
        inventory_has_item(caction->p_item) == 1) {

      for (i = 0; i < MAX_ITEM_DESCRIPTIONS; i++) {
        if (strcmp(caction->p_item->descriptions[i].i_command, caction->in_command) == 0) {
          strcat(output, caction->p_item->descriptions[i].i_description);
          strcat(output, "\n\n");
          return output;
        }
      }
    }

    // for transition / item / npc action commands
    desc_idx = 0;
    while (desc_idx < descriptions_count) {
      if (descriptions[desc_idx].cond.place_id == current_place &&
          strcmp(descriptions[desc_idx].cond.action_command, caction->in_command) == 0) {

        // check status of id item / transition / npc
        has_item = 0;
        has_transition = 0;
        has_npc = 0;
        if (caction->p_item != NULL && caction->p_item->id > 0 &&
            descriptions[desc_idx].cond.elem_type == DESC_ELEM_TYPE_ITEM &&
            descriptions[desc_idx].cond.elem_id == caction->p_item->id) {

          if (descriptions[desc_idx].cond.elem_status == caction->p_item->status.current) {
            has_item = 1;
          }

        } else if (caction->transition != NULL && caction->transition->target_place_id > 0 &&
                   descriptions[desc_idx].cond.elem_type == DESC_ELEM_TYPE_TRANSITION &&
                   descriptions[desc_idx].cond.elem_id == caction->transition->target_place_id) {

          if (descriptions[desc_idx].cond.elem_status == caction->transition->status) {
            // set has transition with valid status only
            if (caction->transition->status == STATUS_TRANSITION_LOCKED &&
                caction->p_item == NULL && descriptions[desc_idx].cond.action_item_id == 0) {
              // use (unlock) command without correct item
              has_transition = 1;

            } else if (caction->transition->status == STATUS_TRANSITION_LOCKED &&
                caction->p_item != NULL &&
                descriptions[desc_idx].cond.action_item_id == caction->p_item->id) {
              // use (unlock) command with correct item
              has_transition = 1;
            } else if (caction->transition->status != STATUS_TRANSITION_LOCKED) {
              // another valid transition status
              has_transition = 1;
            }
          }

        } else if (caction->c_npc != NULL && caction->c_npc->id > 0 &&
                   descriptions[desc_idx].cond.elem_type == DESC_ELEM_TYPE_NPC &&
                   descriptions[desc_idx].cond.elem_id == caction->c_npc->id) {
          has_npc = 1;
        }

        if (has_item == 1 || has_transition == 1 || has_npc == 1) {
          // check status of available text items to hide descriptions
          // of items which have another location than "in place"
          for (i = 0; i < MAX_DESC_TEXT_ITEMS; i++) {
            struct item *citem = get_item_by_id(descriptions[desc_idx].item_ids[i]);
            if (citem != NULL && citem->location != ITEM_LOCATION_PLACE) {
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

      } else if (descriptions[desc_idx].cond.place_id == current_place + 1) {
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

  } else {
    // for dialog commands
    if (dialog_get_current_idx() > 0) {
      strcat(output, dialog_get_output());

    // for simple actions commands
    } else if (strcmp(caction->in_command, "description") == 0) {
      strcat(output, description_by_area_place());

    } else if (strcmp(caction->in_command, "help") == 0) {
      strcat(output, help_text);
      strcat(output, "\n");

    } else if (strcmp(caction->in_command, "inventory") == 0) {
      strcat(output, inventory_get_output(&phrases_data));
    }
  }

  return output;
}
