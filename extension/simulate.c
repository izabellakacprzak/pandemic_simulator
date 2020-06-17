#include "simulate_utils.h"
#include "simulate_social.h"
#include "simulationIO.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "gifoutput/gif_output.h"

typedef enum outputSelection {
	NO_OUTPUT,
	GIF,
	TERMINAL
} outputSelection;

#define CELL_SIZE 20

int main (int argc, char **argv) {
  char input[10];
  int noTurns, gridLength, gridHeight, population, initiallyInfected, numSocials;
  ErrorCode err = OK;
  Disease disease = {0};
  outputSelection outputType = NO_OUTPUT; //no output selected
  
  srand(time(NULL));

  setInitial(&disease, &population, &initiallyInfected,
	     &gridLength, &gridHeight, &numSocials);
  configurate(&disease, &population, &initiallyInfected,
	      &gridLength, &gridHeight, &numSocials);

  int noFreeCells;
  Point *freeCells;

  //creates an array of humans on the heap
  Grid grid = calloc(gridLength, sizeof(GridCell*));

  FATAL_PROG((grid == NULL), ALLOCATION_FAIL);

  for (int i = 0; i < gridLength; i++) {
    grid[i] = calloc(gridHeight, sizeof(GridCell));
    FATAL_PROG((grid[i] == NULL), ALLOCATION_FAIL);
    //creates unoccupied cells of default type
  }

  SocialSpace *socialPlaces;

  if(numSocials){
    socialPlaces = calloc(numSocials, sizeof(SocialSpace));
    initialiseSocials(numSocials, grid, socialPlaces, gridLength, gridHeight);
  }
  
  Human **humans = calloc(population, sizeof(Human*));

  FATAL_PROG((humans == NULL), ALLOCATION_FAIL);


  noFreeCells = gridLength * gridHeight;
  freeCells = calloc(noFreeCells, sizeof(Point));

  FATAL_PROG((freeCells == NULL), ALLOCATION_FAIL);
  // initialized all cell Points to be free
  for(int i = 0; i < gridLength; i++){
	for(int j = 0; j < gridHeight; j++){
		freeCells[i + (j * gridLength)].x = i;
		freeCells[i + (j * gridLength)].y = j;
	}
  }

  int index;
  Point currPoint;
  for (int i = 0; i < population; i++) {
		humans[i] = calloc(1,sizeof(Human));
		FATAL_PROG((humans[i] == NULL), ALLOCATION_FAIL);
		index = RANDINT(0, noFreeCells); 
		currPoint = freeCells[index];
		humans[i]->x = currPoint.x;
		humans[i]->y = currPoint.y;
		//makes sure two humans cant be in the same square

		humans[i]->risk = randomFrom0To1() * 2;
		if (numSocials == 1) {
      			humans[i]->socialPreference = 1;
    		} else if (numSocials) {
		      humans[i]->socialPreference = RANDINT(1, numSocials);
	        }
		CELL_SET(grid[currPoint.x][currPoint.y], humans[i]);

		freeCells[index] = freeCells[noFreeCells - 1];
		noFreeCells--;
		if(noFreeCells > 0){
			freeCells = realloc(freeCells, noFreeCells * sizeof(Point));
		}
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
    printf("How many turns do you want to include?\n");
    scanf("%9s", input);
    noTurns = atoi(input);

    ge_GIF *gif = initialiseGif(gridLength, gridHeight, CELL_SIZE);


    writeFrame(gif, grid, gridLength, gridHeight, CELL_SIZE);
    //adds a frame of the current board to the gif
    
    for (int i = 0; i < noTurns; i++) {
      //call turn function
      moveAStar(grid, humans, population, socialPlaces, gridLength, gridHeight);
      checkInfections(grid, humans, &population, gridLength, gridHeight, &disease);
      writeFrame(gif, grid, gridLength, gridHeight, CELL_SIZE);
    }
    
    //close gif file and produce gif
    ge_close_gif(gif);
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

  if(numSocials){
    free(socialPlaces);
  }
  free(grid);
  free(humans);
  free(freeCells);

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
