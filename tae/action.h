#include "structs.h"

void action_reset();

struct action *action_get();

int action_item_check_comb(struct item *p_item, struct item *s_item);

int action_handle_input(char *input);
