#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "simulate_utils.h"
#include "simulate_social.h"
#include "simulationIO.h"
#include "gifoutput/gif_output.h"

#define CELL_SIZE (20)

int main(void) {

  /* Initialize variables used for input purposes */
  char input[10];
  int noTurns, gridColumns, gridRows, population, initiallyInfected, numSocials, quarantine;
  ErrorCode err = OK;
  Disease disease = {0};
  outputSelection outputType = NO_OUTPUT; 
  configSelection configType = NO_CONFIGURATION; 
  
  /* Seed the random number generator */
  srand(time(NULL));

  /* Set up the configuration file */
  setInitial(&disease, &population, &initiallyInfected,
             &gridColumns, &gridRows, &numSocials, &quarantine);
  configure(&disease, &population, &initiallyInfected,
              &gridColumns, &gridRows, &numSocials, &quarantine);

  printf("The default values the program will be run with are: \n\n");	
  printConfigValues(&disease, &population, &initiallyInfected, 
  	              &gridColumns, &gridRows, &numSocials, &quarantine);
  
  while(configType == NO_CONFIGURATION) {
    printf("Would you like to keep the default variables or use the ones from the config file? ('default'/'config')\n");
    scanf("%9s", input);

    if(strcmp(input, "config") == 0) {
      configType = CONFIG;
    } else if(strcmp(input, "default") != 0) {
      printf("Invalid input %s\n", input);
    }
  }

  if(configType == CONFIG){   
  printf("The new values the program will be run with are: \n");	        
  configure(&disease, &population, &initiallyInfected,
              &gridColumns, &gridRows, &numSocials, &quarantine);
  }

  /* Set up variables that display the changes at the end of the simulation */	  
  int populationStat, sickStat, deadStat, latentStat;
  populationStat = population;
  latentStat = initiallyInfected;
  sickStat = 0;
  
  int noFreeCells;
  Point *freeCells;

  /* Create a grid, array of humans and social places on the heap */		
  Grid grid = calloc(gridRows, sizeof(GridCell *));
  FATAL_PROG((grid == NULL), ALLOCATION_FAIL);

  for(int i = 0; i < gridRows; i++) {
    grid[i] = calloc(gridColumns, sizeof(GridCell));
    FATAL_PROG((grid[i] == NULL), ALLOCATION_FAIL);
  }

  SocialSpace *socialPlaces;

  if(numSocials) {
    socialPlaces = calloc(numSocials, sizeof(SocialSpace));
    initialiseSocials(numSocials, grid, socialPlaces, gridColumns, gridRows);
  }

  Human **humans = calloc(population, sizeof(Human *));
  FATAL_PROG((humans == NULL), ALLOCATION_FAIL);

  /* Initialize all cell Points as free */
  noFreeCells = gridColumns * gridRows;
  freeCells = calloc(noFreeCells, sizeof(Point));
  FATAL_PROG((freeCells == NULL), ALLOCATION_FAIL);

  for(int i = 0; i < gridRows; i++) {
    for(int j = 0; j < gridColumns; j++) {
      freeCells[i * gridColumns + j].y = i;
      freeCells[i * gridColumns + j].x = j;
    }
  }

  /* Populate the grid with humans and assign a 
     social preference and risk factor to each one*/
  int index;
  Point currPoint;
  for(int i = 0; i < population; i++) {
    humans[i] = calloc(1, sizeof(Human));
    FATAL_PROG((humans[i] == NULL), ALLOCATION_FAIL);
    index = RANDINT(0, noFreeCells);
    currPoint = freeCells[index];
    humans[i]->x = currPoint.x;
    humans[i]->y = currPoint.y;
    humans[i]->risk = randomFrom0To1() * 2;

    if(numSocials) {
      humans[i]->socialPreference = RANDINT(0, numSocials);
    }
    /* Ensure two humans cannot be in the same square */
    cellSet(&grid[currPoint.y][currPoint.x], humans[i]);
    freeCells[index] = freeCells[noFreeCells - 1];
    noFreeCells--;
    if(noFreeCells > 0) {
      freeCells = realloc(freeCells, noFreeCells * sizeof(Point));
    }
  }

  /* Set an initial number of humans to be infected */
  for(int i = 0; i < initiallyInfected; i++) {
    humans[i]->status = LATENT;
    humans[i]->latencyTime = disease.latencyPeriod;
  }

  /* Get prefered output format */
  while(outputType == NO_OUTPUT) {
    printf("What output would you like? ('gif'/'terminal')\n");
    scanf("%9s", input);

    if(strcmp(input, "gif") == 0) {
      outputType = GIF;
    } else if(strcmp(input, "terminal") == 0) {
      outputType = TERMINAL;
    } else {
      printf("Invalid input %s\n", input);
    }
  }

  /* To be used in each turn - determines for how long 
     the A* algorithm and the ranom movement one are used */
  int socialTime = -1;
  if(numSocials) {
    socialTime = (gridColumns + gridRows) / (numSocials);
  }
  int socialIndex = 1;
  
  /* Perform the amount of turns specified by 
     the user until the choose to quit */
  if(outputType == TERMINAL) {
    FATAL_SYS(getNextInput(input) != 1); 

    while(strcmp(input, "q")) {
      noTurns = atoi(input);
     
      for (int i = 0; i < noTurns; i++) {
      //call turn function
      	if(socialIndex > 0 && socialIndex < socialTime){
        moveAStar(grid, humans, population, socialPlaces,gridColumns, gridRows, quarantine);
        } else {
        if(socialIndex == socialTime){
          socialIndex = -socialTime * 3 / 2;	
        }
        move(grid, humans, population, gridColumns, gridRows, quarantine);
     }
        checkInfections(grid, humans, &population, &sickStat, &latentStat, gridColumns, gridRows, &disease);
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

    /* Add a frame of the current board to the gif */
    writeFrame(gif, grid, gridColumns, gridRows, CELL_SIZE);
    //adds a frame of the current board to the gif
    
    for (int i = 0; i < noTurns; i++) {
      //call turn function
      	if(socialIndex > 0 && socialIndex < socialTime){
        moveAStar(grid, humans, population, socialPlaces,gridColumns, gridRows, quarantine);
        } else {
        if(socialIndex == socialTime){
          socialIndex = -socialTime * 3 / 2;	
        }
        move(grid, humans, population, gridColumns, gridRows, quarantine);
 	}
	checkInfections(grid, humans, &population, &sickStat, &latentStat, gridColumns, gridRows, &disease); 
	socialIndex++;
        writeFrame(gif, grid, gridColumns, gridRows, CELL_SIZE);
    }
    ge_close_gif(gif);
  }

  deadStat = populationStat - population;
  printf("The stats at the end of the simulation are: \n");
  printf("Initial population: %d\n", populationStat);
  printf("Number of deaths: %d\n", deadStat);
  printf("Number of latent cases: %d\n", latentStat);
  printf("Number of sick cases: %d\n", sickStat);


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

  if(numSocials) {
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
