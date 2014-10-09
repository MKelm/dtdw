#ifndef STRUCTS_DEFINITION
#define STRUCTS_DEFINITION

struct meta {
  char title[256];
  char version[24];
  char author[36];
  int cyear;
};

#define PHRASES_CHARS_LENGTH 256

struct phrases {
  char inv_title[PHRASES_CHARS_LENGTH];
  char no_inv_items[PHRASES_CHARS_LENGTH];
  char items_comb[PHRASES_CHARS_LENGTH];
  char items_comb_failure[PHRASES_CHARS_LENGTH];
  char item_usage_failure[PHRASES_CHARS_LENGTH];
};

#define MAX_COMMAND_LENGTH 36

struct command {
  char in[MAX_COMMAND_LENGTH]; // internal
  char ex[MAX_COMMAND_LENGTH]; // external
};

#define MAX_AREA_TITLE_LENGTH 56

struct area {
  int id;
  char title[MAX_AREA_TITLE_LENGTH];
};

struct placetrans {
  int id;
  char title[56];
  int status; // 0 = open, 1 = closed, 2 = locked
  int ul_item_id; // item id to unlock transition
};

#define MAX_PLACE_TRANSITIONS 5

struct place {
  int area_id;
  int id;
  char title[56];
  struct placetrans transitions[MAX_PLACE_TRANSITIONS];
};

#define MAX_ITEM_TITLE_LENGTH 56

struct item {
  int id;
  int comb_id; // combination item id
  int final_id; // final item id after combination
  char title[MAX_ITEM_TITLE_LENGTH];
  int status; // 0 == in place, 1 == in inventory, -1 == exists no longer
};

#define DIALOG_ELEMENT_MAX_NEXT_IDS 10

struct dialogelement {
  int id;
  char text[1024];
  int next_ids[DIALOG_ELEMENT_MAX_NEXT_IDS];
  int next_mchoice; // status are next ids multiple choice
};

#define DIALOG_MAX_ELEMENTS 100

struct dialog {
  int npc_id;
  int elements_count;
  struct dialogelement elements[DIALOG_MAX_ELEMENTS];
};

#define MAX_NPC_NAME_LENGTH 56

struct npc {
  int id;
  char title[MAX_NPC_NAME_LENGTH];
  int area_id;
  int place_id;
  struct dialog *c_dialog;
};

#define MAX_DESC_ID_EXTRAS 5
#define MAX_DESC_TEXT_TRANS 5
#define MAX_DESC_TEXT_ITEMS 10
#define MAX_DESC_TEXT_NPCS 10

struct description {
  int id;
  int id_transitions[MAX_DESC_ID_EXTRAS];
  int id_trans_status[MAX_DESC_ID_EXTRAS];
  int id_trans_item_id[MAX_DESC_ID_EXTRAS];
  int id_items[MAX_DESC_ID_EXTRAS];
  int id_npcs[MAX_DESC_ID_EXTRAS];
  char id_verb[24];
  char text[1024];
  int transitions[MAX_DESC_TEXT_TRANS]; // text transitions
  int items[MAX_DESC_TEXT_ITEMS]; // text items
  int npcs[MAX_DESC_TEXT_NPCS];
};

struct action {
  char in_command[256]; // internal command
  struct placetrans *transition;
  struct item *pitem; // primary item
  struct item *sitem; // sub item
  struct item *f_item; // final item after item combinition
  struct npc *c_npc; // current npc
};

#endif
