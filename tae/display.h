#include "structs.h"

void dsp_init(void);
void dsp_windows_init(void);

void dsp_set_meta(struct meta *meta_data);
void dsp_set_location(struct area *area_data, struct place *place_data);

char *dsp_input(void);
void dsp_output(char *str);

void dsp_end(void);
