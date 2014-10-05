#include "structs.h"

// define how many action descriptions will be visible at maximum
#define MAX_ACTIONS 5

void load_data(void);
void actions_init(struct action actions[], int lmax);

int *get_area_place_idx(void);
char *get_output();
char *actions_get_output(struct action actions[], int lmax, char *output);

struct action get_input_action(char *input);
char *get_internal_command(char *ccommand);
int get_object_id(char *tobject);
