#include <stdio.h>
#include <string.h>
#include "inventory.h"

struct inventory_item {
  struct item *link;
} inventory_items[100];

int inventory_count = 0;

int inventory_has_item(struct item *link) {
  int i;
  for (i = 0; i < inventory_count; i++) {
    if (inventory_items[i].link->id == link->id) {
      return 1;
    }
  }
  return 0;
}

int inventory_add_item(struct item *link) {
  if (link != NULL && link->id > 0) {
    link->status = 1; // set status to "in inventory"
    inventory_items[inventory_count].link = link;
    inventory_count++;
    return 1;
  }
  return 0;
}

int inventory_rm_item(struct item *link) {
  int i = 0, removed = 0;
  for (i = 0; i < inventory_count; i++) {
    if (inventory_items[i].link->id == link->id) {
      link->status = -1; // set status to "exists no longer"
      removed = 1;
    }
    if (removed == 1) {
      inventory_items[i].link = inventory_items[i+1].link;
    }
  }
  if (removed == 1)
    inventory_count--;
  return removed;
}

char *inventory_get_output(struct phrases *phrases_data) {
  char line[1024];
  static char output[1024];
  strncpy(output, "", sizeof(output));
  if (inventory_count == 0) {
    snprintf(
      output, 1024, "%s: %s\n\n", phrases_data->inv_title, phrases_data->no_inv_items
    );
  } else {
    snprintf(output, 1024, "%s:", phrases_data->inv_title);
    int i;
    for (i = 0; i < inventory_count; i++) {
      snprintf(
        line, 1024, " %s%s", inventory_items[i].link->title,
        (i+1 < inventory_count) ? "," : ""
      );
      strcat(output, line);
    }
    strcat(output, "\n\n");
  }
  return output;
}
