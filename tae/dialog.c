#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dialog.h"

int dialog_element_idx = -1;
struct dialog *c_dialog;
int is_multiple_choice = 0;

void dialog_set_current(struct dialog *ptr_dialog) {
  dialog_element_idx = 0;
  c_dialog = ptr_dialog;
}

char *dialog_get_next_output(void) {
  static char output[1024], line[1024];
  int has_next_ids = 0;
  strncpy(output, "", sizeof(output));
  if (dialog_element_idx > -1) {
    // first output
    snprintf(output, 1024, "\"%s\"\n", c_dialog->elements[dialog_element_idx].text);
    if (c_dialog->elements[dialog_element_idx].next_mchoice == 1) {
      // multiple choice output
      is_multiple_choice = 1; // flag for input mode
      has_next_ids = 1;
    } else if (c_dialog->elements[dialog_element_idx].next_ids[0] > 0) {
      // regular output
      is_multiple_choice = 0;
      has_next_ids = 1;
    }
    if (has_next_ids == 1) {
      int i;
      for (i = 0; i < DIALOG_ELEMENT_MAX_NEXT_IDS; i++) {
        if (c_dialog->elements[dialog_element_idx].next_ids[i] > 0) {

          dialog_element_idx = dialog_get_next_element_idx(
            c_dialog->elements[dialog_element_idx].next_ids[i]
          );
          if (is_multiple_choice == 1) {
            snprintf(line, 1024, "*  \"%s\" (%d)\n",
              c_dialog->elements[dialog_element_idx].text, i+1);
          } else {
            snprintf(line, 1024, "\"%s\"\n",
              c_dialog->elements[dialog_element_idx].text);
          }
          strcat(output, line);
        }
      }
      strcat(output, "\n");
    }
  }
  return output;
}

int dialog_get_next_element_idx(int id) {
  int idx;
  for (idx = dialog_element_idx; idx < DIALOG_MAX_ELEMENTS; idx++) {
    if (c_dialog->elements[idx].id == id) {
      return idx;
    }
  }
  return dialog_element_idx;
}

void dialog_set_input(char *input) {
  if (strlen(input) > 0) {
    if (is_multiple_choice == 1) {
      int choice_id = atoi(input);
      // handle choice decision todo ...
    }
  }
}
