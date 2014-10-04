#include "structs.h"

// meta
#define FILE_META "../data/meta.txt"

void load_meta(struct meta *data);

// commands
#define FILE_COMMANDS "../data/commands.txt"
#define MAX_COMMANDS 20

int load_commands(struct command data[], int lmax);

// areas
#define FILE_AREAS "../data/areas.txt"
#define MAX_AREAS 50

int load_areas(struct area data[], int lmax);

// places
#define FILE_PLACES "../data/places.txt"
#define MAX_PLACES 200

int load_places(struct place data[], int lmax);

// objects
#define FILE_OBJECTS "../data/objects.txt"
#define MAX_OBJECTS 500

int load_objects(struct object data[], int lmax);

// descriptions
#define FILE_DESCRIPTIONS "../data/descriptions.txt"
#define MAX_DESCRIPTIONS 1000

int load_descriptions(struct description data[], int lmax);
