#include <string.h>
#include "main.h"
#include "loader.h"
#include "display.h"

int counts[5];

struct meta meta_data;
struct command commands[MAX_COMMANDS];
struct area areas[MAX_AREAS];
struct place places[MAX_PLACES];
struct object objects[MAX_OBJECTS];
struct description descriptions[MAX_DESCRIPTIONS];

int main(void) {
  load_data();

  dsp_init();

  dsp_windows_init();
  dsp_set_meta(&meta_data);

  char output[256] = "Something in the output #window#, and something other than my dog in window fight. But in all situations it is the best to perform a $rabbit$. It is like any other game here and there. Motivations are good to have so we go further and further.";
  dsp_output(output);

  char *input;
  do {
    input = dsp_input();
  } while (strcmp(input, "~") != 0);

  dsp_end();
  return 0;
}

void load_data(void) {
  // meta
  load_meta(&meta_data);
  // commands
  counts[0] = load_commands(commands, MAX_COMMANDS);
  // areas
  counts[1] = load_areas(areas, MAX_AREAS);
  // places
  counts[2] = load_places(places, MAX_PLACES);
  // objects
  counts[3] = load_objects(objects, MAX_OBJECTS);
  // descriptions
  counts[4] = load_descriptions(descriptions, MAX_DESCRIPTIONS);
}
