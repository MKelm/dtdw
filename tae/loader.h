#include "structs.h"

#define LOADER_DATA_FOLDER "data/"

void loader_set_data_dir(void);
void loader_set_area_id(int area_id);

// help
#define FILE_HELP "help.json"
#define MAX_HELP_TEXT_CHARS 2048

void load_help(char *text);

// meta
#define FILE_META "meta.json"
#define MAX_META_LINE_CHARS 1024

void load_meta(struct meta *data);

// phrases

#define FILE_PHRASES "phrases.txt"

void load_phrases(struct phrases *data) ;

// commands
#define FILE_COMMANDS "commands.txt"
#define MAX_COMMANDS 20

int load_commands(struct command data[], int lmax);

// areas
#define FILE_AREAS "areas.txt"
#define MAX_AREAS 50

int load_areas(struct area data[], int lmax);

// places
#define FILE_PLACES "places.txt"
#define MAX_PLACES 200

int load_places(struct place data[], int lmax);
int load_places_rec(FILE *f, struct place *data, int data_idx);

// items
#define FILE_ITEMS "items.txt"
#define MAX_ITEMS 500

int load_items(struct item data[], int lmax);

// npcs
#define FILE_NPCS "npcs.txt"
#define MAX_NPCS 500

int load_npcs(struct npc data[], int lmax);

#define FILE_DIALOGS_FOLDER "dialogs/"
#define FILE_DIALOGS_POSTFIX ".txt"
#define MAX_DIALOGS 500

int load_dialogs(struct npc npcs_data[], int nlmax, struct dialog data[], int lmax);

// descriptions
#define FILE_DESCRIPTIONS "descriptions.txt"
#define MAX_DESCRIPTIONS 1000

int load_descriptions(struct description data[], int lmax);
int load_descriptions_rec(FILE *f, struct description *data, int data_idx);

// transitions
#define MAX_PLACETRANS 1000

int load_transitions(struct placetrans transitions_data[], int transitions_lmax,
                     struct place places_data[], int places_lmax, int placetrans_lmax);
