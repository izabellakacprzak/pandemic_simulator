#include "simulate_utils.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GRID_SIZE 10

void printToTerminal(Grid grid, int gridLength, int gridHeight) {
  assert(gridLength >= 0);
  assert(gridHeight >= 0);

  GridCell cell;

  for (int i = 0; i < gridHeight; i++) {
    for (int j = 0; j < gridLength; j++) {
      cell = grid[j][i]; //assuming array goes [x][y]
      if (cell.human) {
	switch (cell.human->status) {
	case HEALTHY:
	  printf("H"); //healthy
	  break;
	case INFECTED:
	  printf("C"); //carrier
	  break;
	case INFECTED_WITH_SYMPTOMS:
	  printf("I"); //infected
	  break;
	default:
	  printf("E"); //error
	  break;
	}
      } else {
	printf("-"); //empty cell
      }
    }
    printf("\n");
  }
}

int getNextInput(char *input) {
  printf("How many turns do you want to wait? (q to quit)\n");
  return scanf("%9s", input);
}

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
