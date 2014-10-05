#include "structs.h"

int inventory_add_item(struct item *link);
int inventory_rm_item(struct item *link);

char *inventory_get_output(struct meta *meta_data);
