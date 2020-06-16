#include "simulate_utils.h"
#include "simulationIO.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef enum outputSelection {
  NO_OUTPUT,
  GIF,
  TERMINAL
} outputSelection;

int main (int argc, char **argv) {
  char input[10];
  int noTurns, gridLength, gridHeight, population, initiallyInfected;
  ErrorCode err = OK;
  Disease disease = {0};
  outputSelection outputType = NO_OUTPUT; //no output selected
  
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

  while(outputType == NO_OUTPUT) {
    printf("What output would you like? ('gif'/'terminal')\n");
    scanf("%9s", input);

    if (strcmp(input, "gif") == 0) {
      outputType = GIF;
    } else if (strcmp(input, "terminal") == 0) {
      outputType = GIF;
    } else {
      printf("Invalid input %s\n", input);
    }
  }

  if (outputType == TERMINAL) {
    FATAL_SYS(getNextInput(input) != 1); //kill if no item is scanned
  
    while (strcmp(input, "q")) {
      noTurns = atoi(input);
      
      for (int i = 0; i < noTurns; i++) {
      //call turn function
	move(grid, humans, population, gridLength, gridHeight);
	checkInfections(grid, humans, &population, gridLength, gridHeight, &disease);
      }
      
      printToTerminal(grid, gridLength, gridHeight);

      FATAL_SYS(getNextInput(input) != 1);
    }
  } else {
    //TODO add gif handling code
    printf("How many turns do you want to include?\n");
    scanf("%9s", input);
    noTurns = atoi(input);
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
