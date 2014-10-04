#include "data.h"

// meta
#define FILE_META "../def/meta.txt"

void load_meta(struct meta *meta_data);

// commands
#define FILE_COMMANDS "../def/commands.txt"
#define MAX_COMMANDS 12

int load_commands(struct command commands[], int lmax);

// areas
#define FILE_AREAS "../def/areas.txt"
#define MAX_AREAS 50

int load_areas(struct area areas[], int lmax);

// places
#define FILE_PLACES "../def/places.txt"
#define MAX_PLACES 200

int load_places(struct place places[], int lmax);

// objects
#define FILE_OBJECTS "../def/objects.txt"
#define MAX_OBJECTS 500

int load_objects(struct object objects[], int lmax);

// descriptions
#define FILE_DESCRIPTIONS "../def/descriptions.txt"
#define MAX_DESCRIPTIONS 1000

int load_descriptions(struct description descriptions[], int lmax);
