#include <stdint.h>
#include <stdlib.h>
#include "gifenc.h"
#include "gif_output.h"

//loads an rgb value into a colour buffer at a given index
static void addColour(uint8_t *buffer, int index, uint8_t r, uint8_t g, uint8_t b) {
  buffer[index] = r;
  buffer[index+1] = g;
  buffer[index+2] = b;
}

ge_GIF *initialiseGif(int gridLength, int gridHeight, int cellSize) {
  uint16_t width = gridLength * cellSize;
  uint16_t height = gridHeight * cellSize;

  uint8_t palette[24];

  addColour(palette, 3*WHITE, 0xFF, 0xFF, 0xFF); //white = 0
  addColour(palette, 3*BLACK, 0x00, 0x00, 0x00); //black = 1
  addColour(palette, 3*RED, 0xFF, 0x00, 0x00); //red = 2
  addColour(palette, 3*GREEN, 0x00, 0xFF, 0x00); //green = 3
  addColour(palette, 3*BLUE, 0x00, 0x00, 0xFF); //blue = 4
  addColour(palette, 3*YELLOW, 0xFF, 0xFF, 0x00); //yellow = 5
  addColour(palette, 3*PURPLE, 0xFF, 0x00, 0xFF); //purple = 6
  addColour(palette, 3*TURQUOISE, 0xFF, 0x00, 0xFF); //turquoise = 7

  int depth = 3;
  
  ge_GIF *gif = ge_new_gif("pandemic.gif", width, height, palette, depth, 0);
  //creates a gif with a custom colour palette that loops infinitely
  return gif;
}

static void drawCell(ge_GIF *gif, int x, int y, int cellSize, uint8_t currentPixel) {
  int width = gif->w;
  int base = cellSize * (y * width + x);
  for (int i = 0; i < cellSize; i++) {
    for (int j = 0; j < cellSize; j++) {
      gif->frame[base + (width * i + j)] = currentPixel;
    }
  }
}

void writeFrame(ge_GIF *gif, Grid grid, int gridLength, int gridHeight, int cellSize) {
  Colour currentColour;
  uint8_t currentPixel;
  for (int i = 0; i < gridLength; i++) {
    for (int j = 0; j < gridHeight; j++) {

      if (!grid[i][j].human) {
	switch(grid[i][j].type) {
	case NORMAL:
	  currentColour = WHITE;
	  break;
	case WORK:
	  currentColour = YELLOW;
	  break;
	case HOSPITAL:
	  currentColour = PURPLE;
	  break;
	default:
	  currentColour = BLACK;
	}
      } else {
	switch (grid[i][j].human->status) {
	case HEALTHY:
	  currentColour = GREEN;
	  break;
	case LATENT:
	  currentColour = BLUE;
	  break;
	case SICK:
	  currentColour = RED;
	  break;
	default:
	  currentColour = BLACK;
	}
      }
      currentPixel = currentColour; //converts to 8 bit value
      drawCell(gif, i , j, cellSize, currentPixel);
      //draws a square of length cellSize into the gif
      
    }
  }
  ge_add_frame(gif, FRAME_DELAY);
}



