#include <stdio.h>
#include <string.h>
#include <locale.h>
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
  setlocale (LC_ALL, "");

  load_data();

  dsp_init();

  dsp_windows_init();
  dsp_set_meta(&meta_data);
  dsp_set_location(&areas_data[0], &places_data[0]);

  char *output = get_output();
  dsp_output(output);

  char *input;
  do {
    input = dsp_input();
  } while (strcmp(input, "~") != 0);

  dsp_end();
  return 0;
}

char *get_output(void) {
  // get first output with intro by using virtual description
  int desc_idx = 0;
  static char line[1024], output[1024];
  if (current_place == 0 && descriptions_data[desc_idx].id == 0) {
    snprintf(output, 1024, "%s\n\n", descriptions_data[desc_idx].text);
    current_area = 1;
    current_place = 1;
    desc_idx++;
  }

  // get more data of the current area place
  while (desc_idx < data_counts[4]) {
    if (descriptions_data[desc_idx].id == current_place &&
        strlen(descriptions_data[desc_idx].id_verb) == 0) {

      snprintf(line, 1024, "%s ", descriptions_data[desc_idx].text);
      if (desc_idx > 0) {
        strcat(output, line);
      } else {
        snprintf(output, 1024, "%s ", descriptions_data[desc_idx].text);
      }
    } else if (descriptions_data[desc_idx].id == current_place + 1) {
      return output;
    }
    desc_idx++;
  }

  return output;
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
