#include "data.h"

void dsp_init(void);
void dsp_windows_init(void);

void dsp_set_meta(struct meta *meta_data);

char *dsp_input(void);
void dsp_output(char *str);

void dsp_end(void);
