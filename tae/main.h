#include "structs.h"

// define how many action descriptions will be visible at maximum
// todo: handle only one action at one time to avoid duplicate outputs
#define MAX_ACTIONS 100

void load_data(void);
void actions_init(struct action actions[], int lmax);

int *get_area_place_idx(void);
char *desc_get_output();
char *actions_get_output(struct action actions[], int lmax);

struct action get_input_action(char *input);
char *get_internal_command(char *ccommand);
int get_object_id(char *tobject);
