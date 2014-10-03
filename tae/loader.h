#define FILE_META "../def/meta.txt"
#define FILE_DESCRIPTIONS "../def/descriptions.txt"

struct meta {
  char title[256];
  char version[24];
  char author[36];
  int cyear;
};

void load_meta(struct meta *meta_data);

struct description {
  int id;
  int id_items[5];
  char id_verb[24];
  char text[1024];
  int transitions[5]; // text transitions
  int items[10]; // text items
};

int load_descriptions(struct description descriptions[], int lmax);
