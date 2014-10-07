#include "structs.h"

void dialog_set_current(struct dialog *ptr_dialog);

char *dialog_get_output(void);

int dialog_get_next_element_idx(int current_idx, int next_id);

void dialog_set_input(char *input);
