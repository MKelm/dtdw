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
  static char output[1024];
  int has_next_ids = 0;
  if (dialog_element_idx == 0) {
    // first output, todo change logic to handle next ids directly
    snprintf(output, 1024, "\"%s\"\n\n", c_dialog->elements[dialog_element_idx].text);
    return output;
  } else if (dialog_element_idx > 0 &&
             c_dialog->elements[dialog_element_idx-1].next_mchoice == 1) {
    // multiple choice output
    is_multiple_choice = 1; // flag for input mode
    has_next_ids = 1;
  } else if (c_dialog->elements[dialog_element_idx-1].next_ids[0] > 0) {
    // regular output
    is_multiple_choice = 0;
    has_next_ids = 1;
  }
  if (has_next_ids == 1) {
    int i;
    for (i = 0; i < DIALOG_ELEMENT_MAX_NEXT_IDS; i++) {
      if (c_dialog->elements[dialog_element_idx-1].next_ids[i] > 0) {

        struct dialogelement *c_element = dialog_get_element(
          c_dialog->elements[dialog_element_idx-1].next_ids[i]
        );
        if (is_multiple_choice == 1) {
          snprintf(output, 1024, "*  \"%s\" (%d)\n", c_element->text, i+1);
        } else {
          snprintf(output, 1024, "\"%s\"\n", c_element->text);
        }
      }
    }
    strcat(output, "\n");
    return output;
  }
  return "";
}

struct dialogelement *dialog_get_element(int id) {
  int i;
  for (i = 0; i < DIALOG_MAX_ELEMENTS; i++) {
    if (c_dialog->elements[i].id == id)
      return &c_dialog->elements[i];
  }
  return NULL;
}

void dialog_set_input(char *input) {
  if (strlen(input) > 0) {
    if (is_multiple_choice == 1) {
      int choice_id = atoi(input);
      // handle choice decision todo ...
    }
  }
}
