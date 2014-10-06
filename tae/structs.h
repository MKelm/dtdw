#ifndef STRUCTS_DEFINITION
#define STRUCTS_DEFINITION

struct meta {
  char title[256];
  char version[24];
  char author[36];
  int cyear;
  char tinventory[24];
  char noinvitems[42];
};

struct command {
  char in[24]; // internal
  char ex[24]; // external
};

struct area {
  int id;
  char title[56];
};

struct placetrans {
  int id;
  char title[56];
  int locked;
};

#define MAX_PLACE_TRANSITIONS 5

struct place {
  int area_id;
  int id;
  char title[56];
  struct placetrans transitions[MAX_PLACE_TRANSITIONS];
};

struct item {
  int id;
  int comb_id; // combination item id
  int final_id; // final item id after combination
  char title[56];
  int status; // 0 == in place, 1 == in inventory, -1 == exists no longer
};

struct dialogelement {
  int id;
  char text[1024];
  int next_ids[10];
  int next_mchoice; // status are next ids multiple choice
};

struct dialog {
  int npc_id;
  struct dialogelement elements[100];
};

struct npc {
  int id;
  char title[56];
  int area_id;
  int place_id;
  struct dialog *cdialog;
};

#define MAX_DESC_ID_EXTRAS 5
#define MAX_DESC_TEXT_TRANS 5
#define MAX_DESC_TEXT_ITEMS 10

struct description {
  int id;
  int id_transitions[MAX_DESC_ID_EXTRAS];
  int id_items[MAX_DESC_ID_EXTRAS];
  char id_verb[24];
  char text[1024];
  int transitions[MAX_DESC_TEXT_TRANS]; // text transitions
  int items[MAX_DESC_TEXT_ITEMS]; // text items
};

struct action {
  char in_command[256]; // internal command
  struct placetrans *transition;
  struct item *pitem; // primary item
  struct item *sitem; // sub item
};

#endif
