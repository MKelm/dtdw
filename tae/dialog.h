#include "structs.h"

void dialog_set_current(struct dialog *ptr_dialog);

char *dialog_get_next_output(void);

struct dialogelement *dialog_get_element(int id);

void dialog_set_input(char *input);
