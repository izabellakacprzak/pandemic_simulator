
#ifndef GIF_OUTPUT
#define GIF_OUTPUT

#include "gifenc.h"
#include "../lib/simulate_utils.h"

#define FRAME_DELAY 10

typedef enum Colours {
  WHITE,
  BLACK,
  RED,
  GREEN,
  BLUE,
  YELLOW,
  PURPLE,
  TURQUOISE  
} Colour;

ge_GIF *initialiseGif(int gridColumns, int gridRows, int cellSize);

void writeFrame(ge_GIF *gif, Grid grid, int gridColumns, int gridRows, int cellSize);

#endif
