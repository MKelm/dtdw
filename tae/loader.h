#define FILE_META "../def/meta.txt"

struct meta {
  char title[256];
  char version[24];
  char author[36];
  int cyear;
};

void load_meta(struct meta *meta_data);

struct command {
  char in[24]; // internal
  char ex[24]; // external
};

#define FILE_COMMANDS "../def/commands.txt"
#define MAX_COMMANDS 12

int load_commands(struct command commands[], int lmax);

struct description {
  int id;
  int id_items[5];
  char id_verb[24];
  char text[1024];
  int transitions[5]; // text transitions
  int items[10]; // text items
};

#define FILE_DESCRIPTIONS "../def/descriptions.txt"
#define MAX_DESCRIPTIONS 1000

int load_descriptions(struct description descriptions[], int lmax);
