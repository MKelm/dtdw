#include <string.h>
#include "main.h"
#include "loader.h"
#include "display.h"

int current_area = 0;
int current_place = 0;

int data_counts[5];
struct meta meta_data;
struct command commands_data[MAX_COMMANDS];
struct area areas_data[MAX_AREAS];
struct place places_data[MAX_PLACES];
struct object objects_data[MAX_OBJECTS];
struct description descriptions_data[MAX_DESCRIPTIONS];

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
  data_counts[0] = load_commands(commands_data, MAX_COMMANDS);
  // areas
  data_counts[1] = load_areas(areas_data, MAX_AREAS);
  // places
  data_counts[2] = load_places(places_data, MAX_PLACES);
  // objects
  data_counts[3] = load_objects(objects_data, MAX_OBJECTS);
  // descriptions
  data_counts[4] = load_descriptions(descriptions_data, MAX_DESCRIPTIONS);
}
