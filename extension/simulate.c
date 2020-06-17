#include "simulate_utils.h"
#include "simulate_social.h"
#include "simulationIO.h"
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
  int noTurns, gridColumns, gridRows, population, initiallyInfected, numSocials;
  ErrorCode err = OK;
  Disease disease = {0};
  outputSelection outputType = NO_OUTPUT; //no output selected
  
  srand(time(NULL));

  setInitial(&disease, &population, &initiallyInfected,
             &gridColumns, &gridRows, &numSocials);
  configurate(&disease, &population, &initiallyInfected,
              &gridColumns, &gridRows, &numSocials);

  int noFreeCells;
  Point *freeCells;

  //creates an array of humans on the heap
  Grid grid = calloc(gridRows, sizeof(GridCell*));

  FATAL_PROG((grid == NULL), ALLOCATION_FAIL);

  for (int i = 0; i < gridRows; i++) {
    grid[i] = calloc(gridColumns, sizeof(GridCell));
    FATAL_PROG((grid[i] == NULL), ALLOCATION_FAIL);
    //creates unoccupied cells of default type
  }

  SocialSpace *socialPlaces;

  if(numSocials){
    socialPlaces = calloc(numSocials, sizeof(SocialSpace));
    initialiseSocials(numSocials, grid, socialPlaces, gridColumns, gridRows);
  }
  
  Human **humans = calloc(population, sizeof(Human*));

  FATAL_PROG((humans == NULL), ALLOCATION_FAIL);


  noFreeCells = gridColumns * gridRows;
  freeCells = calloc(noFreeCells, sizeof(Point));

  FATAL_PROG((freeCells == NULL), ALLOCATION_FAIL);
  // initialized all cell Points to be free
  for(int i = 0; i < gridRows; i++){
	for(int j = 0; j < gridColumns; j++){
		freeCells[i * gridColumns + j].y = i;
		freeCells[i * gridColumns + j].x = j;
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
		if (numSocials) {
		     humans[i]->socialPreference = RANDINT(0, numSocials);
	    }
		cellSet(&grid[currPoint.y][currPoint.x], humans[i]);
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
      outputType = TERMINAL;
    } else {
      printf("Invalid input %s\n", input);
    }
  }

  int socialTime = (gridColumns + gridRows) / (numSocials);
  int socialIndex = 1;
  if (outputType == TERMINAL) {
    FATAL_SYS(getNextInput(input) != 1); //kill if no item is scanned
  
    while (strcmp(input, "q")) {
      noTurns = atoi(input);
      
      for (int i = 0; i < noTurns; i++) {
      //call turn function
      	if(socialIndex > 0 && socialIndex < socialTime){
        moveAStar(grid, humans, population, socialPlaces,gridColumns, gridRows);
        } else {
        if(socialIndex == socialTime){
          socialIndex = -socialTime * 3 / 2;	
        }
        move(grid, humans, population, gridColumns, gridRows);
        }
        checkInfections(grid, humans, &population, gridColumns, gridRows, &disease);
        socialIndex++;
      }

      printToTerminal(grid, gridColumns, gridRows);

      FATAL_SYS(getNextInput(input) != 1);
    }
  } else {
    printf("How many turns do you want to include?\n");
    scanf("%9s", input);
    noTurns = atoi(input);

    ge_GIF *gif = initialiseGif(gridColumns, gridRows, CELL_SIZE);


    writeFrame(gif, grid, gridColumns, gridRows, CELL_SIZE);
    //adds a frame of the current board to the gif
    
    for (int i = 0; i < noTurns; i++) {
      //call turn function
      	if(socialIndex > 0 && socialIndex < socialTime){
        moveAStar(grid, humans, population, socialPlaces,gridColumns, gridRows);
        } else {
        if(socialIndex == socialTime){
          socialIndex = -socialTime * 3 / 2;	
        }
        move(grid, humans, population, gridColumns, gridRows);
        }
        checkInfections(grid, humans, &population, gridColumns, gridRows, &disease);
        socialIndex++;
        writeFrame(gif, grid, gridColumns, gridRows, CELL_SIZE);
    }
    
    //close gif file and produce gif
    ge_close_gif(gif);
  }

 fatalError:

  if(grid) {
    for(int i = 0; i < gridRows; i++) {
      free(grid[i]);
    }
  }

  if(humans) {
    for(int i = 0; i < population; i++) {
      if(humans[i]) {
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

  if(err != OK) {
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
