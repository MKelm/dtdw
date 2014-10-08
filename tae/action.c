#include <stdio.h>
#include <string.h>
#include "action.h"
#include "main.h"

struct action c_action;

void action_reset() {
  strcpy(c_action.in_command, "");
  c_action.transition = NULL;
  c_action.pitem = NULL;
  c_action.sitem = NULL;
  c_action.f_item = NULL;
  c_action.c_npc = NULL;
}

struct action *action_get() {
  return &c_action;
}

int action_handle_input(char *input) {
  action_reset();

  char inputarr[4][24], commandarr[4][24];
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
            strncpy(commandarr[j], ptr, sizeof(commandarr[j]));
            ptr = strtok(NULL, " ");
            j++;
          }
        }
        if (j > 0) {
          if (i == 2) {
            // more actions for item/transition action command
            strncpy(c_action.in_command, commandarr[0], sizeof(c_action.in_command));
            c_action.pitem = get_item(inputarr[1]);
            if (c_action.pitem == NULL) {
              c_action.transition = get_transition(inputarr[1]);
            }

            if (c_action.pitem == NULL && c_action.transition == NULL) {
              c_action.c_npc = get_npc(inputarr[1]);
            }
            return 1;
          } else if (i == 4) {
            // more actions for item combinition action command
            strncpy(c_action.in_command, commandarr[0], sizeof(c_action.in_command));
            c_action.pitem = get_item(inputarr[1]);
            c_action.sitem = get_item(inputarr[3]);

            // check try item combination
            if (c_action.pitem != NULL && c_action.sitem != NULL) {
              if (main_item_check_comb(c_action.pitem, c_action.sitem) == 0) {
                action_reset();
                return 0;
              } else {
                c_action.f_item = get_item_by_id(c_action.pitem->final_id);
              }
            } else if (c_action.pitem != NULL) {
              // check try item / transition combination
              c_action.transition = get_transition(inputarr[3]);
            }
            return 1;
          }
        }
      } else if (i == 1) {
        // for simple actions
        strncpy(c_action.in_command, icommand, sizeof(c_action.in_command));
        return 1;
      }
    }
  }
  return 0;
}
