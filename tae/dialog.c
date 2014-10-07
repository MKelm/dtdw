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

char *dialog_get_output(void) {
  static char output[1024];
  int has_next_ids = 0;
  strncpy(output, "", sizeof(output));
  if (dialog_element_idx > -1) {
    // first output
    snprintf(output, 1024, "! \"%s\"\n", c_dialog->elements[dialog_element_idx].text);
    has_next_ids = dialog_check_element_has_next_ids(dialog_element_idx);
    is_multiple_choice = dialog_check_element_is_mchoice(dialog_element_idx);

    if (has_next_ids == 1) {
      dialog_get_next_element_output(output, dialog_element_idx, is_multiple_choice);
    }
  }
  return output;
}

int dialog_check_element_has_next_ids(int dialog_element_idx) {
  if (c_dialog->elements[dialog_element_idx].next_ids[0] > 0)
    return 1;
  return 0;
}

int dialog_check_element_is_mchoice(int dialog_element_idx) {
  // multiple choice output
  if (c_dialog->elements[dialog_element_idx].next_mchoice > 0)
    return 1; // flag for input mode
  return 0;
}

void dialog_get_next_element_output(
       char *output, int dialog_element_idx, int is_multiple_choice
     ) {
  int i, next_id_idx = 0;
  char line[1024];
  for (i = 0; i < DIALOG_ELEMENT_MAX_NEXT_IDS; i++) {
    if (c_dialog->elements[dialog_element_idx].next_ids[i] > 0) {
      next_id_idx = dialog_get_next_element_idx(
         dialog_element_idx, c_dialog->elements[dialog_element_idx].next_ids[i]
      );

      if (is_multiple_choice == 1) {
        snprintf(line, 1024, "?  \"%s\" (%d)\n",
          c_dialog->elements[next_id_idx].text, i+1);
      } else {
        snprintf(line, 1024, "! \"%s\"\n",
          c_dialog->elements[next_id_idx].text);
      }
      strcat(output, line);
    }
  }
  strcat(output, "\n");
  if (dialog_check_element_has_next_ids(next_id_idx) == 1) {
    is_multiple_choice = dialog_check_element_is_mchoice(next_id_idx);
    if (is_multiple_choice == 1) {
      // set current element idx to last multiple choice element
      // change to next element idx by input selection value
      dialog_element_idx = next_id_idx;
      dialog_get_next_element_output(output, dialog_element_idx, is_multiple_choice);
    }
  }
}

int dialog_get_next_element_idx(int dialog_element_idx, int next_id) {
  int idx;
  for (idx = dialog_element_idx; idx < DIALOG_MAX_ELEMENTS; idx++) {
    if (c_dialog->elements[idx].id == next_id) {
      return idx;
    }
  }
  return idx;
}

void dialog_set_input(char *input) {
  if (strlen(input) > 0) {
    if (is_multiple_choice == 1) {
      // int choice_id = atoi(input);
      // handle choice decision todo ...
    }
  }
}
