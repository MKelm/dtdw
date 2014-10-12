#include "structs.h"

void load_data(void);
void init_action(struct action *caction);

int *get_area_place_idx(void);

char *get_internal_command(char *ccommand);

struct item *get_item(char *titel);
struct item *get_item_by_id(int id);
struct placetrans *get_transition(char *title);
struct npc *get_npc(char *title);
