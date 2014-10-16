DTDW
====

"Der Talisman der Wächterin" is a german text adventure project. If do not speak
German the IDX.tae (Text Adventure Engine) might be interesting for you. The
engine related source code and contents are in english.

## Changelog

### v0.0.6 alpha
- tae: added logic to handle item status in place descriptions
- tae: added item status values with options and changes to use it
- tae: changes to use pulled/pushed item status
- tae: some minor bugfixes and improvements in loader and data structures
- dtdw: added a third room for tests
- dtdw: some corrections in json data

### v0.0.5 alpha
- tae: added jsmn to parse json data files
- tae: changed all loader functions to handle json data files
- tae: improved description data structure
- tae: improved some const values for status conditions
- dtdw: changed text data files to json data files
- dtdw: splitted description data files by place id

### v0.0.4 alpha
- tae: added logic to push/pull items
- tae: added logic to give items to npcs
- tae: added optional logic to get items from npc
- tae: inventory item descriptions related to commands
- tae: area specific data folders
- dtdw: some minor changes to use/test the new features

### v0.0.3 alpha
- tae: added logic to combine items
- tae: added logic to handle locked/closes status for transitions
- tae: added logic to unlock transitions by using an item
- tae: added logic to open/close transitions
- tae: added some phrases, e.g. to use in inventory
- tae: some minor improvements, e.g. simplified loader logic
- dtdw: some minor changes to use/test the new features

### v0.0.2 alpha
- tae: added logic for npcs
- tae: added logic for npc dialogs
- tae: moved some main logic to seperate action / description module
- dtdw: added first npc data and dialog data
- dtdw: some minor changes / additions in descriptions

### v0.0.1 alpha
- tae: added logic for descriptions
- tae: added logic to move around
- tae: added logic to lookat items/transitions
- tae: added logic to pickup items / handle inventory
- dtdw: added one area with two places and transitions
- dtdw: added some items with descriptions by status/action
- dtdw: added commands, help text and meta data
