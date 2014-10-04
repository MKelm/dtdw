#include "structs.h"

void load_data(void);

int *get_area_place_idx(void);
char *get_output(void);

void check_input_command(char *input, struct action *caction);
char *get_internal_command(char *ccommand);
int get_object_id(char *tobject);
