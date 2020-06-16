#include "simulate_utils.h"
#include "simulationIO.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


int main (int argc, char **argv) {
  char input[10];
  int noTurns, gridLength, gridHeight, population, initiallyInfected;
  ErrorCode err = OK;
  Disease disease = {0};

  srand(time(NULL));

  setInitial(&disease, &population, &initiallyInfected,
	     &gridLength, &gridHeight);
  configurate(&disease, &population, &initiallyInfected,
	      &gridLength, &gridHeight);

  //creates an array of humans on the heap
  Grid grid = calloc(gridLength, sizeof(GridCell*));

  FATAL_PROG((grid == NULL), ALLOCATION_FAIL);

  for (int i = 0; i < gridLength; i++) {
    grid[i] = calloc(gridHeight, sizeof(GridCell));
    FATAL_PROG((grid[i] == NULL), ALLOCATION_FAIL);
    //creates unoccupied cells of default type
  }

  Human **humans = calloc(population, sizeof(Human*));

  FATAL_PROG((humans == NULL), ALLOCATION_FAIL);

  int x, y;
  for (int i = 0; i < population; i++) {
    humans[i] = calloc(1,sizeof(Human));
    FATAL_PROG((humans[i] == NULL), ALLOCATION_FAIL);
    do{
      x = RANDINT(0, gridLength - 1); 
      y = RANDINT(0, gridHeight - 1);
    } while (grid[x][y].human);
    //makes sure two humans cant be in the same square

    humans[i]->x = x;
    humans[i]->y = y;
    humans[i]->risk = randomFrom0To1();
    CELL_SET(grid[x][y], humans[i]);
  }

  //sets an initial number of humans to be infected
  for(int i = 0; i < initiallyInfected; i++) {
    humans[i]->status = LATENT;
    humans[i]->latencyTime = disease.latencyPeriod;
  }

  getNextInput(input);
  
  while (strcmp(input, "q")) {
    noTurns = atoi(input);

    for (int i = 0; i < noTurns; i++) {
      //call turn function
      move(grid, humans, population, gridLength, gridHeight);
      checkInfections(grid, humans, &population, gridLength, gridHeight, &disease);
    }

    printToTerminal(grid, gridLength, gridHeight);

    getNextInput(input);
  }

 fatalError:

  if (grid) {
    for (int i = 0; i < gridLength; i++) {
      free(grid[i]);
    }
  }

  if (humans) {
    for (int i = 0; i < population; i++) {
      if (humans[i]) {
        free(humans[i]);
      }
    }
  }
  
  free(grid);
  free(humans);

  if (err != OK) {
    /* Print an error message*/
    char *errorMessage;
    if(EC_IS_SYS_ERROR(err)) {
      errorMessage = strerror(EC_TO_SYS_ERROR(err));
    } else {
      errorMessage = getProgramError(err);
    }
    printf("%s\n", errorMessage);
    return EXIT_FAILURE;
  }
  
  return EXIT_SUCCESS;
}
