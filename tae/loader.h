// meta
#define FILE_META "../def/meta.txt"

struct meta {
  char title[256];
  char version[24];
  char author[36];
  int cyear;
};

void load_meta(struct meta *meta_data);

// commands
#define FILE_COMMANDS "../def/commands.txt"
#define MAX_COMMANDS 12

struct command {
  char in[24]; // internal
  char ex[24]; // external
};

int load_commands(struct command commands[], int lmax);

// areas
#define FILE_AREAS "../def/areas.txt"
#define MAX_AREAS 50

struct area {
  int id;
  char title[56];
};

int load_areas(struct area areas[], int lmax);

// places
#define FILE_PLACES "../def/places.txt"
#define MAX_PLACES 200

struct place {
  int area_id;
  int id;
  char title[56];
};

int load_places(struct place places[], int lmax);

// descriptions
#define FILE_DESCRIPTIONS "../def/descriptions.txt"
#define MAX_DESCRIPTIONS 1000

struct description {
  int id;
  int id_items[5];
  char id_verb[24];
  char text[1024];
  int transitions[5]; // text transitions
  int items[10]; // text items
};

int load_descriptions(struct description descriptions[], int lmax);
