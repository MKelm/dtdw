#include <string.h>
#include "display.h"
#include "main.h"

int main(void) {
  dsp_init();

  dsp_windows_init();

  char output[256] = "Something in the output #window#, and something other than my dog in window fight. But in all situations it is the best to perform a $rabbit$. It is like any other game here and there. Motivations are good to have so we go further and further.";
  dsp_output(output);

  char *input;
  do {
    input = dsp_input();
  } while (strcmp(input, "~") != 0);

  dsp_end();
  return 0;
}