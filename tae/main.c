#include <stdio.h>
#include <string.h>
#include <locale.h>
#include "main.h"
#include "loader.h"
#include "display.h"

int current_area = 1;
int current_place = 1;

// dc: 0 == commands, 2 == areas, 3 == places, 4 == objects, 5 == descriptions
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

  int location_change = 1, output_change = 1;
  int *area_place_idx;
  char *output, *input;

  do {
    // check location change, change header display output if needed
    if (location_change == 1) {
      area_place_idx = get_area_place_idx();
      dsp_set_location(
        &areas_data[area_place_idx[0]], &places_data[area_place_idx[1]]
      );
      location_change = 0;
    }
    // check output change, change output display window if needed
    if (output_change == 1) {
      output = get_output();
      dsp_set_output(output);
      output_change = 0;
    }
    // input prompt
    input = dsp_get_input();
  } while (strcmp(input, "~") != 0);

  dsp_end();
  return 0;
}

int *get_area_place_idx(void) {
  static int area_place_idx[2];
  int areaidx = 0, placeidx = 0;

  int i = 0, run = 1;
  while (run == 1) {
    if (areas_data[i].id == current_area) {
      areaidx = i;
      run = 0;
    } else
     i++;
  }
  i = 0, run = 1;
  while (run == 1) {
    if (places_data[i].area_id == current_area &&
        places_data[i].id == current_place) {
      placeidx = i;
      run = 0;
    } else
     i++;
  }

  area_place_idx[0] = areaidx;
  area_place_idx[1] = placeidx;
  return area_place_idx;
}

char *get_output(void) {
  // get first output with intro by using virtual description
  int desc_idx = 0, has_text = 0;;
  static char line[1024], output[1024];
  if (current_area == 1 && current_place == 1 && descriptions_data[desc_idx].id == 0) {
    snprintf(output, 1024, "%s\n\n", descriptions_data[desc_idx].text);
    has_text = 1;
    desc_idx++;
  }

  // get more data of the current area place
  while (desc_idx < data_counts[4]) {
    if (descriptions_data[desc_idx].id == current_place &&
        strlen(descriptions_data[desc_idx].id_verb) == 0) {

      snprintf(line, 1024, "%s ", descriptions_data[desc_idx].text);
      if (has_text == 1) {
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
