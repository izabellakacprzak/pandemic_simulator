#include "simulate_utils.h"
#include "simulationIO.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main (int argc, char **argv) {
  Grid grid = calloc(GRID_SIZE, sizeof(GridCell*));
  char input[10];
  int noTurns;
  
  for (int i = 0; i < GRID_SIZE; i++) {
    grid[i] = calloc(GRID_SIZE, sizeof(GridCell));
  }

  getNextInput(input);
  
  while (strcmp(input, "q")) {
    noTurns = atoi(input);

    for (int i = 0; i < noTurns; i++) {
      //call turn function
    }

    printToTerminal(grid, GRID_SIZE, GRID_SIZE);

    getNextInput(input);
  }
  
  for (int i = 0; i < GRID_SIZE; i++) {
    free(grid[i]);
  }

  free(grid);
  return 0;
}
