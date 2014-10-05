#include "structs.h"

void load_data(void);
void init_action(struct action *caction);

int *get_area_place_idx(void);
char *desc_get_output();
char *inventory_get_output(void);
char *action_get_output(struct action *caction);

struct action get_input_action(char *input);
char *get_internal_command(char *ccommand);
int get_item_id(char *titem);
int get_transition_id(char *ttransition);
