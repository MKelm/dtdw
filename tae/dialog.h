#include "structs.h"

void dialog_set_current(struct dialog *ptr_dialog);

char *dialog_get_next_output(void);

int dialog_get_next_element_idx(int id);

void dialog_set_input(char *input);
