#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "dialog.h"

int dialog_element_idx = -1;
struct dialog *c_dialog;
int is_multiple_choice = 0;

int current_choice[DIALOG_ELEMENT_MAX_NEXT_IDS];

void dialog_set_current(struct dialog *ptr_dialog) {
  dialog_element_idx = 0;
  c_dialog = ptr_dialog;
}

// to check if dialog mode is active
int dialog_get_current_idx(void) {
  return dialog_element_idx;
}

void dialog_close() {
  is_multiple_choice = 0;
  dialog_element_idx = -1;
  c_dialog = NULL;
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
      dialog_get_next_element_output(output);
    }
  }
  return output;
}

int dialog_check_element_has_next_ids(int current_element_idx) {
  if (c_dialog->elements[current_element_idx].next_ids[0] > 0)
    return 1;
  return 0;
}

int dialog_check_element_is_mchoice(int current_element_idx) {
  // multiple choice output
  if (c_dialog->elements[current_element_idx].next_mchoice > 0)
    return 1; // flag for input mode
  return 0;
}

void dialog_init_current_choice(void) {
  int i;
  for (i = 0; i < DIALOG_ELEMENT_MAX_NEXT_IDS; i++) {
    current_choice[i] = -1;
  }
}

void dialog_get_next_element_output(char *output) {
  int i, next_id_idx = 0;
  char line[1024];
  if (is_multiple_choice == 1)
    dialog_init_current_choice();

  for (i = 0; i < DIALOG_ELEMENT_MAX_NEXT_IDS; i++) {
    if (c_dialog->elements[dialog_element_idx].next_ids[i] > 0) {
      next_id_idx = dialog_get_next_element_idx(
         dialog_element_idx, c_dialog->elements[dialog_element_idx].next_ids[i]
      );

      if (is_multiple_choice == 1) {
        snprintf(line, 1024, "?  \"%s\" (%d)\n",
          c_dialog->elements[next_id_idx].text, i+1);
        current_choice[i] = next_id_idx;
      } else {
        snprintf(line, 1024, "! \"%s\"\n",
          c_dialog->elements[next_id_idx].text);
      }
      strcat(output, line);
    }
  }
  strcat(output, "\n");
  if (dialog_check_element_has_next_ids(next_id_idx) == 1) {
    // output for next multiple choice answers only
    if (dialog_check_element_is_mchoice(next_id_idx) == 1) {
      is_multiple_choice = 1;
      // set current element idx to last multiple choice element
      // change to next element idx by input selection value
      dialog_element_idx = next_id_idx;
      dialog_get_next_element_output(output);
    }
  } else if (is_multiple_choice == 0) {
    dialog_close();
  }
}

int dialog_get_next_element_idx(int current_element_idx, int next_id) {
  int idx;
  for (idx = current_element_idx; idx < DIALOG_MAX_ELEMENTS; idx++) {
    if (c_dialog->elements[idx].id == next_id) {
      return idx;
    }
  }
  return idx;
}

// returns 0 or 1 for output_change
int dialog_handle_input(char *input) {
  if (is_multiple_choice == 1) {
    if (strlen(input) > 0) {
      // get next element id by input selection
      int i, current_choice_id = atoi(input) - 1;
      for (i = 0; i < DIALOG_ELEMENT_MAX_NEXT_IDS; i++) {
        if (i == current_choice_id) {
          dialog_element_idx = current_choice[i];
          return 1;
        }
      }
    }
  }
  return 0;
}
