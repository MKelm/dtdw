#include "structs.h"

// help
#define FILE_HELP "../data/help.txt"

void load_help(char *text);

// meta
#define FILE_META "../data/meta.txt"

void load_meta(struct meta *data);

// phrases

#define FILE_PHRASES "../data/phrases.txt"

void load_phrases(struct phrases *data) ;

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

// items
#define FILE_ITEMS "../data/items.txt"
#define MAX_ITEMS 500

int load_items(struct item data[], int lmax);

// npcs
#define FILE_NPCS "../data/npcs.txt"
#define MAX_NPCS 500

int load_npcs(struct npc data[], int lmax);

#define FILE_DIALOGS_FOLDER "../data/dialogs/"
#define FILE_DIALOGS_POSTFIX ".txt"
#define MAX_DIALOGS 500

int load_dialogs(struct npc npcs_data[], int nlmax, struct dialog data[], int lmax);

// descriptions
#define FILE_DESCRIPTIONS "../data/descriptions.txt"
#define MAX_DESCRIPTIONS 1000

int load_descriptions(struct description data[], int lmax);

// transitions
#define MAX_PLACETRANS 1000

int load_transitions(struct placetrans transitions_data[], int transitions_lmax,
                     struct place places_data[], int places_lmax, int placetrans_lmax);
