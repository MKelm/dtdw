#include "structs.h"

int inventory_has_item(struct item *link);
int inventory_add_item(struct item *link);
int inventory_rm_item(struct item *link);

char *inventory_get_output(struct phrases *phrases_data);
