#include "structs.h"
#include "jsmn/jsmn.h"

#define LOADER_DATA_FOLDER "data/"
#define MAX_JSON_LINE_CHARS 1024

void loader_set_data_dir(void);
void loader_set_area_id(int area_id);

FILE *loader_get_data_file(char *file_name, short in_area, short place_id);
void load_json(FILE *f, char *output, int output_length, jsmntok_t *tokens, int tokens_length);
void load_json_token(char* input, char *token, jsmntok_t *tokens, int token_idx);

// help
#define FILE_HELP "help.json"
#define MAX_HELP_TEXT_CHARS 2048

void load_help(char *text);

// meta
#define FILE_META "meta.json"
#define MAX_META_LINE_CHARS 1024

void load_meta(struct meta *data);

// phrases

#define FILE_PHRASES "phrases.json"
#define MAX_PHRASES_LINE_CHARS 1024

void load_phrases(struct phrases *data) ;

// commands
#define FILE_COMMANDS "commands.json"
#define MAX_COMMANDS_LINE_CHARS 1024
#define MAX_COMMANDS 20

int load_commands(struct command data[], int lmax);

// areas
#define FILE_AREAS "areas.json"
#define MAX_AREAS 50

int load_areas(struct area data[], int lmax);

// places
#define FILE_PLACES "places.json"
#define MAX_PLACES_LINE_CHARS 1024
#define MAX_PLACES 200

int load_places(struct place data[], int lmax);

// items
#define FILE_ITEMS "items.json"
#define MAX_ITEMS 500

int load_items(struct item data[], int lmax);
void load_items_element(char *output, jsmntok_t *tokens, int *i,
                        int *data_idx, struct item *data);
void load_items_element_descs(char *output, jsmntok_t *tokens, int *i,
                              int *data_idx, struct item *data);
void load_item_element_status(char *output, jsmntok_t *tokens, int *i,
                              int *data_idx, struct item *data);
void load_item_element_status_options(char *output, jsmntok_t *tokens, int *i,
                                      int *data_idx, struct item *data);
int load_item_status_value(char *input);

// npcs
#define FILE_NPCS "npcs.json"
#define MAX_NPCS 500

int load_npcs(struct npc data[], int lmax);

#define FILE_DIALOGS_FOLDER "dialogs/"
#define FILE_DIALOGS_POSTFIX ".json"
#define MAX_DIALOGS 500

int load_dialogs(struct npc npcs_data[], int nlmax, struct dialog data[], int lmax);

// intro
#define FILE_INTRO "intro.json"
#define MAX_INTRO_TEXT_CHARS 2048

void load_intro(char *text);

// descriptions
#define FILE_DESCRIPTIONS "descriptions.json"
#define MAX_DESCRIPTIONS 1000

int load_descriptions(struct description *data, int lmax,
                      struct place *places_data, int places_lmax);

// transitions
#define MAX_PLACETRANS 1000

int load_transitions(struct placetrans transitions_data[], int transitions_lmax,
                     struct place places_data[], int places_lmax, int placetrans_lmax);
