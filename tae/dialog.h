#include "structs.h"

void dialog_set_current(struct dialog *ptr_dialog);
int dialog_get_current_idx(void);
void dialog_close();

char *dialog_get_output(void);
void dialog_get_next_element_output(char *output);
int dialog_get_next_element_idx(int current_element_idx, int next_id);

int dialog_check_element_has_next_ids(int current_element_idx);
int dialog_check_element_is_mchoice(int current_element_idx);
void dialog_init_current_choice(void);

int dialog_handle_input(char *input);
