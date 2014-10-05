#ifndef STRUCTS_DEFINITION
#define STRUCTS_DEFINITION

struct action {
  char in_command[256]; // internal command
  int transition_id;
  int pitem_id; // primary item id
  int sitem_id; // sub item id
};

struct meta {
  char title[256];
  char version[24];
  char author[36];
  int cyear;
};

struct command {
  char in[24]; // internal
  char ex[24]; // external
};

struct area {
  int id;
  char title[56];
};

struct place {
  int area_id;
  int id;
  char title[56];
};

struct object {
  int id;
  int comb_id; // combination object id
  int final_id; // final object id after combination
  char title[56];
};

#define MAX_DESC_ID_EXTRAS 5

struct description {
  int id;
  int id_transitions[MAX_DESC_ID_EXTRAS];
  int id_items[MAX_DESC_ID_EXTRAS];
  char id_verb[24];
  char text[1024];
  int transitions[5]; // text transitions
  int items[10]; // text items
};

#endif
