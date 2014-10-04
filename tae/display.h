#include "structs.h"

void dsp_init(void);
void dsp_windows_init(void);

void dsp_set_meta(struct meta *meta_data);
void dsp_set_location(struct area *area_data, struct place *place_data);
void dsp_set_output(char *str);

char *dsp_word_wrap(char* buffer, char* string, int line_width);
char *dsp_get_input(void);

void dsp_end(void);
