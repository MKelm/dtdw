#include "structs.h"

void dialog_set_current(struct dialog *ptr_dialog);

char *dialog_get_output(void);
void dialog_get_next_element_output(char *output, int dialog_element_idx, int is_multiple_choice);
int dialog_get_next_element_idx(int dialog_element_idx, int next_id);

int dialog_check_element_has_next_ids(int dialog_element_idx);
int dialog_check_element_is_mchoice(int dialog_element_idx);

void dialog_set_input(char *input);
