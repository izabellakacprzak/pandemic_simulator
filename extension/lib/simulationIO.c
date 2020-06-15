#include "simulationIO.h"
#include <stdio.h>
#include <assert.h>
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
	case LATENT:
	  printf("L"); //carrier
	  break;
	case SICK:
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
