#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <time.h>

#include "simulate_utils.h"
#include "simulate_social.h"
#include "simulationIO.h"

static void executeRand(int executions) {
  assert(executions >= 0);

  while (executions) {
    rand();
    executions--;
  }
}

static void resetHumans(int newPopulation, int *population, Human **humans,
			Grid grid, int gridLength, int gridHeight, int *noFreeCells,
			Point **freeCells, int numSocials) {
  for (int i = 0; i < *population; i++) {
    free(humans[i]);
  }

  for (int i = 0; i < gridLength; i++) {
    for (int j = 0; j < gridHeight; j++) {
      grid[j][i].human = NULL;
    }
  }

  free(*freeCells);
  
  int newNoFreeCells = gridLength * gridHeight;
  Point *newFreeCells = calloc(newNoFreeCells, sizeof(Point));

  // initialized all cell Points to be free
  for(int i = 0; i < gridHeight; i++){
    for(int j = 0; j < gridLength; j++){
      newFreeCells[(i * gridLength) + j].y = i;
      newFreeCells[(i * gridLength) + j].x = j;
    }
  }

  int index;
  Point currPoint;
  for (int i = 0; i < newPopulation; i++) {
    humans[i] = calloc(1,sizeof(Human));
    
    index = RANDINT(0, newNoFreeCells); 
    currPoint = newFreeCells[index];
    humans[i]->x = currPoint.x;
    humans[i]->y = currPoint.y;
    //makes sure two humans cant be in the same square

    humans[i]->risk = randomFrom0To1() * 2;
    if (numSocials) {
      humans[i]->socialPreference = RANDINT(0, numSocials);
    }
    cellSet(&grid[currPoint.y][currPoint.x], humans[i]);

    newFreeCells[index] = newFreeCells[newNoFreeCells - 1];
    newNoFreeCells--;
    if(newNoFreeCells > 0){
      newFreeCells = realloc(newFreeCells, newNoFreeCells * sizeof(Point));
    }
  }

  *population = newPopulation;
  *noFreeCells = newNoFreeCells;
  *freeCells = newFreeCells;
}

static void resetDisease(Disease *disease) {
  disease->latencyPeriod = 0;
  disease->infectionChance = 0;
  disease->fatalityChance = 0;
  disease->recoveryChance = 0;
  disease->immunity = 0;
}

static void changeHumanLocation(int x, int y, Human *human, Grid grid) {
  grid[human->y][human->x].human = NULL;
  grid[y][x].human = human;
  human->x = x;
  human->y = y;
}

static void resetSocialPlaces(int newNumSocials, int *numSocials,
			      SocialSpace **socialPlaces, Grid grid) {
  for (int i = 0; i < *numSocials; i++) {
    grid[socialPlaces[0][i].y][socialPlaces[0][i].x].type = NORMAL;
  }
  
  free(*socialPlaces);
  *socialPlaces = calloc(newNumSocials, sizeof(SocialSpace));
  *numSocials = newNumSocials;
}

static void placeSocialSpace(int x, int y, SocialSpace *socialPlace, Grid grid) {
  grid[y][x].type = SOCIAL;
  socialPlace->x = x;
  socialPlace->y = y;
}

static void testbool(bool condition, const char *testname) {
  printf("T %s: %s\n", testname, condition?"OK":"FAIL");
}


static void testint(int got, int expected, char *testname) {
  printf("T %s (expected=%d, got=%d): %s\n",
	 testname, expected, got, expected==got?"OK":"FAIL");
}

int main(void) {
  /* SET SEED TO RANDOM */
  srand(time(NULL));

  /* ORIGINAL CONFIGS */
  int population = 10;
  int initiallyInfected = 5;
  int quarantine = 0;

  Disease disease = {0};
  disease.latencyPeriod = 7;
  disease.infectionChance = 0.5;
  disease.fatalityChance = 0.1;

  int populationStat = population;
  int deadStat = 0;
  int sickStat = 0;
  int latentStat = initiallyInfected;

  int gridLength = 7; // Note: DO NOT CHANGE for initialiseSocials tests (must be >= 3)
  int gridHeight = 10; // Note: DO NOT CHANGE for initialiseSocials tests (must be >= 3)

  int numSocials = 2;

  /* PRINT CONFIGS */
  printf("Population is: %d\n", population);
  printf("The number of initially infected is: %d\n", initiallyInfected);
  printf("The latency period of the virus is: %d\n", disease.latencyPeriod);
  printf("The infection rate is: %lf\n", disease.infectionChance);
  printf("The fatality rate is: %lf\n", disease.fatalityChance);
  printf("The grid is of size: (%i, %i)\n", gridLength, gridHeight);
  printf("The number of social spaces is: %i\n", numSocials);
  printf("\n");

  /* INITIALISE VARIABLES */
  Grid grid = calloc(gridHeight, sizeof(GridCell *));

  for (int i = 0; i < gridHeight; i++) {
    grid[i] = calloc(gridLength, sizeof(GridCell));
  }
  
  SocialSpace *socialPlaces;

  if(numSocials){
    socialPlaces = calloc(numSocials, sizeof(SocialSpace));
    initialiseSocials(numSocials, grid, socialPlaces, gridLength, gridHeight);
  }

  Human **humans = calloc(population, sizeof(Human *));

  int noFreeCells = gridLength * gridHeight;

 Point *freeCells = calloc(noFreeCells, sizeof(Point));

  for(int i = 0; i < gridHeight; i++){
    for(int j = 0; j < gridLength; j++){
      freeCells[i * gridLength + j].y = i;
      freeCells[i * gridLength + j].x = j;
    }
  }

  int index;
  Point currPoint;
  for (int i = 0; i < population; i++) {
    humans[i] = calloc(1,sizeof(Human));
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

  /* INITIALISATION TESTS */
  printf("Initialisation Tests (Note: initialiseSocials is tested in simulate_social.c tests):\n");

  // --- //
  {
    bool ok = true;
    int locationsX[population], locationsY[population];
    for (int i = 0; i < population; i++) {
      for (int j = 0; j < i; j++) {
	if (humans[i]->x == locationsX[j] && humans[i]->y == locationsY[j]) {
	  ok = false;
	}
      }

      locationsX[i] = humans[i]->x;
      locationsY[i] = humans[i]->y;
    }

    testbool(ok, "No humans in the same location");
  }

  testint(noFreeCells, (gridLength * gridHeight) - population,
	  "noFreeCells decreases for every human added");

  {
    int infected = 0;
    for (int i = 0; i < population; i++) {
      if (humans[i]->status == LATENT) {
	infected++;
      }
    }

    testint(infected, initiallyInfected, "Initially infected humans");
  }

  printf("\n");

  /* SIMULATE_UTILS.C TESTS */
  printf("SIMULATE_UTILS.C:\n");

  /* Test: move(Grid grid, Human **humans, int population,
     int length, int height) */
  printf("Tests for move (simulate_utils.c):\n");

  // population = 1, reset rand
  resetHumans(1, &population, humans, grid, gridLength, gridHeight, &noFreeCells, &freeCells, numSocials);
  srand(1);

  executeRand(1);

  // --- //
  changeHumanLocation(1, 1, humans[0], grid);
  // randoms: {0, -1}
  // x should set to x + 0 = 1
  // y should set to y - 1 = 0
  move(grid, humans, population, gridLength, gridHeight, quarantine);

  testbool(humans[0]->x == 1 && humans[0]->y == 0, "Correct human location change");
  testbool(!grid[1][1].human, "Previous grid cell cleared");
  testbool(grid[humans[0]->y][humans[0]->x].human == humans[0],
	   "Destination cell occupied by the right human");

  executeRand(4);

  // --- //
  changeHumanLocation(0, 0, humans[0], grid);
  // randoms: {-1, -1, 0, 1}
  // {-1, -1} should be ignored
  // x should set to x + 0 = 0
  // y should set to y + 1 = 1
  move(grid, humans, population, gridLength, gridHeight, quarantine);

  testbool(humans[0]->x >= 0 && humans[0]->y >= 0,
	   "Human cannot be placed outside of grid (left and top)");

  executeRand(9);

  // --- //
  changeHumanLocation(gridLength - 1, gridHeight - 1, humans[0], grid);
  // randoms: {1, 1, -1, -1}
  // {1, 1} should be ignored
  // x should set to x - 1 = gridLength - 1
  // y should set to y - 1 = gridHeight - 1
  move(grid, humans, population, gridLength, gridHeight, quarantine);

  testbool(humans[0]->x < gridLength && humans[0]->y < gridHeight,
	   "Human cannot be placed outside of grid (right and bottom)");

  // --- //
  resetHumans(2, &population, humans, grid, gridLength, gridHeight, &noFreeCells, &freeCells, numSocials);
  changeHumanLocation(1, 1, humans[0], grid);
  changeHumanLocation(2, 0, humans[1], grid);

  // randoms: {1, -1, -1, -1, 1, -1, 0, 0, 0, 0}
  // {1, -1} should be ignored by humans[0] ([2][0] is occupied)
  // humans[0]->x should be x - 1 = 0
  // humans[0]->y should be y - 1 = 0
  move(grid, humans, population, gridLength, gridHeight, quarantine);

  testbool(!(humans[0]->x == humans[1]->x && humans[0]->y == humans[1]->y),
	   "Human cannot move to an occupied space");

  printf("\n");
  
  /* Test: checkInfections(Grid grid, Human **humans, int population,
     int length, int height, Disease *disease) */
  printf("Tests for checkInfections (simulate_utils.c):\n");

  // --- //
  resetHumans(1, &population, humans, grid, gridLength, gridHeight, &noFreeCells, &freeCells, numSocials);
  resetDisease(&disease);
  disease.infectionChance = 1;
  disease.latencyPeriod = 10;
  // fatalityChance and recoveryChance = 0
  humans[0]->status = HEALTHY;

  checkInfections(grid, humans, &population, &sickStat, &latentStat, gridLength, gridHeight, &disease);

  testbool(humans[0]->status == HEALTHY, "Single healthy human remains healthy");

  // --- //
  resetHumans(9, &population, humans, grid, gridLength, gridHeight, &noFreeCells, &freeCells, numSocials);
  for (int i = 0; i < population; i++) {
    changeHumanLocation(i % 3, i / 3, humans[i], grid);
    humans[i]->risk = 0;
    humans[i]->status = HEALTHY;
  }
  /* Visual representation:
     0 1 2 -
     3 4 5 - (rest of grid)
     6 7 8 - 
     - - -   */

  humans[4]->status = LATENT;
  checkInfections(grid, humans, &population, &sickStat, &latentStat, gridLength, gridHeight, &disease);

  {
    int latent = 0;
    for (int i = 0; i < population; i++) {
      if (i != 4 && humans[i]->status == LATENT) {
	latent++;
      }
    }

    testint(latent, 8, "Adjacent humans of a LATENT human are made LATENT [risk = 0]");
  }


  // --- //
  for (int i = 0; i < population; i++) {
    humans[i]->risk = 0;
    humans[i]->status = HEALTHY;
  }

  humans[8]->status = LATENT;

  checkInfections(grid, humans, &population, &sickStat, &latentStat, gridLength, gridHeight, &disease);

  {
    int correct = 0;
    for (int i = 0; i < population; i++) {
      if (i == 4 || i == 5 || i == 7) {
	if (humans[i]->status == LATENT) {
	  correct++;
	}
      } else if (i != 8 && humans[i]->status == HEALTHY) {
	correct++;
      }
    }

    testint(correct, 8,
	    "Only adjacent humans to a LATENT human are made LATENT [risk = 0]");
  }

  // --- //
  for (int i = 0; i < population; i++) {
    humans[i]->risk = 0;
    humans[i]->status = HEALTHY;
  }
  
  humans[4]->status = SICK;
  resetDisease(&disease);
  disease.infectionChance = 1;
  disease.latencyPeriod = 10;
  
  checkInfections(grid, humans, &population, &sickStat, &latentStat, gridLength, gridHeight, &disease);

  {
    int latent = 0;
    for (int i = 0; i < population; i++) {
      if (i != 4 && humans[i]->status == LATENT) {
	latent++;
      }
    }

    testint(latent, 8,
	    "All humans who have contant with SICK humans become LATENT [infectionChance = 1]");
  }

  // --- //
  for (int i = 0; i < population; i++) {
    humans[i]->risk = 0;
    humans[i]->status = HEALTHY;
  }

  humans[8]->status = SICK;

  checkInfections(grid, humans, &population, &sickStat, &latentStat, gridLength, gridHeight, &disease);

  {
    int correct = 0;
    for (int i = 0; i < population; i++) {
      if (i == 4 || i == 5 || i == 7) {
	if (humans[i]->status == LATENT) {
	  correct++;
	}
      } else if (i != 8 && humans[i]->status == HEALTHY) {
	correct++;
      }
    }

    testint(correct, 8,
	    "Only adjacent humans to a SICK human are made LATENT [infectionChance = 1]");
  }

  // --- //
  resetDisease(&disease);
  
  resetHumans(10, &population, humans, grid, gridLength, gridHeight, &noFreeCells, &freeCells, numSocials);

  for(int i = 0; i < population; i++){
    humans[i]->status = LATENT;
    humans[i]->latencyTime = 10;
  }

  checkInfections(grid, humans, &population, &sickStat, &latentStat, gridLength, gridHeight, &disease);
  
  {
    int latent = 0;
    for (int i = 0; i < population; i++) {
      if (humans[i]->status == LATENT) {
	latent++;
      }
    }

    testint(latent, 10,
	    "All humans with latencyTime > 0 remain LATENT [latencyTime = 10]");
  }

  {
    int correctTime = 0;
    for (int i = 0; i < population; i++) {
      if (humans[i]->latencyTime == 9) {
	correctTime++;
      }
    }

    testint(correctTime, 10,
	    "All LATENT humans' latencyTime is decreased [latencyTime = 10]");
  }
  
  // --- //
  for(int i = 0; i < population; i++){
    humans[i]->status = LATENT;
    humans[i]->latencyTime = 0;
    humans[i]->risk = 0;
  }

  checkInfections(grid, humans, &population, &sickStat, &latentStat, gridLength, gridHeight, &disease);
  
  {
    int recovered = 0;
    for (int i = 0; i < population; i++) {
      if (humans[i]->status == HEALTHY) {
	recovered++;
      }
    }

    testint(recovered, 10,
	    "All low risk humans recover from LATENT to HEALTHY [risk = 0, latencyTime = 0]");
  }

  // --- //
  for(int i = 0; i < population; i++){
    humans[i]->status = LATENT;
    humans[i]->latencyTime = 0;
    humans[i]->risk = 1;
  }

  checkInfections(grid, humans, &population, &sickStat, &latentStat, gridLength, gridHeight, &disease);

  {
    int sick = 0;
    for (int i = 0; i < population; i++) {
      if (humans[i]->status == SICK) {
	sick++;
      }
    }

    testint(sick, 10,
	    "All high risk humans change from LATENT to SICK [risk = 1, latencyTime = 0]");
  }
  
  // --- //
  resetDisease(&disease);
  disease.fatalityChance = 1;
  
  for(int i = 0; i < population; i++){
    humans[i]->status = SICK;
  }

  checkInfections(grid, humans, &population, &sickStat, &latentStat, gridLength, gridHeight, &disease);

  testint(10 - population, 10,
	  "All SICK humans should be DEAD (i.e. population = 0) [fatalityChance = 1]");

  {
    int count = 0;
    for (int i = 0; i < gridLength; i++) {
      for (int j = 0; j < gridHeight; j++) {
	if (grid[j][i].human) {
	  count++;
	}
      }
    }
  
    testint(10 - population - count, 10, "All DEAD humans are removed from grid");
  }
  
  // --- //
  resetHumans(10, &population, humans, grid, gridLength, gridHeight, &noFreeCells, &freeCells, numSocials);
  resetDisease(&disease);
  disease.recoveryChance = 1;
  
  for(int i = 0; i < population; i++){
    humans[i]->status = SICK;
  }

  checkInfections(grid, humans, &population, &sickStat, &latentStat, gridLength, gridHeight, &disease);

  {
    int healthy = 0;
    for (int i = 0; i < population; i++) {
      if (humans[i]->status == HEALTHY) {
	healthy++;
      }
    }

    testint(healthy, 10,
	  "All SICK humans should recover to HEALTHY [fatalityChance = 0, recoveryChance = 1]");
  }
	
  printf("\n");

  /* SIMULATE_SOCIAL.C TESTS */
  printf("SIMULATE_SOCIAL.C:\n");

  /* Test: initialiseSocials(int amount, Grid grid, SocialSpace *socialPlaces,
		       int gridLength, int gridHeight) */
  printf("Tests for initialiseSocials (simulate_social.c):\n");

  resetSocialPlaces(5, &numSocials, &socialPlaces, grid);
  srand(1);
  executeRand(10); // next in initialiseSocials are (1, 5), (1, 5), (4, 5) (when grid size is (7, 10))

  initialiseSocials(numSocials, grid, socialPlaces, gridLength, gridHeight);

  testbool(!(socialPlaces[0].x == socialPlaces[1].x && socialPlaces[0].y == socialPlaces[1].y),
	   "Two social spaces cannot be placed in the same position");

  {
    int count = 0;
    for (int i = 0; i < gridLength; i++) {
      for (int j = 0; j < gridHeight; j++) {
	if (grid[j][i].type == SOCIAL) {
	  count++;
	}
      }
    }

    testint(count, numSocials, "Correct number of grid cells are SOCIAL");
  }

  {
    int count = 0;
    for (int i = 0; i < numSocials; i++) {
      if (grid[socialPlaces[i].y][socialPlaces[i].x].type == SOCIAL) {
	count++;
      }
    }

    testint(count, numSocials, "socialPlaces x and y values are assigned correctly");
  }

  printf("\n");

  /* Test: moveAStar(Grid grid, Human **humans, int population, SocialSpace *socialPlaces,
                     int length, int height) */
  printf("Tests for moveAStar (simulate_social.c):\n");

  // population = 1, numSocials = 1
  resetSocialPlaces(1, &numSocials, &socialPlaces, grid);
  resetHumans(1, &population, humans, grid, gridLength, gridHeight, &noFreeCells, &freeCells, numSocials);

  // --- //
  placeSocialSpace(1, 1, &socialPlaces[0], grid);
  changeHumanLocation(1, 2, humans[0], grid);
  humans[0]->socialPreference = 0;
  
  moveAStar(grid, humans, population, socialPlaces, gridLength, gridHeight, quarantine);

  testbool(humans[0]->x == 1 && humans[0]->y == 1, "Human moves toward social space");
  testbool(!grid[2][1].human, "Previous grid cell cleared");
  testbool(grid[humans[0]->y][humans[0]->x].human == humans[0],
	   "Destination cell occupied by the right human");

  // --- //
  resetHumans(2, &population, humans, grid, gridLength, gridHeight, &noFreeCells, &freeCells, numSocials);
  changeHumanLocation(1, 2, humans[0], grid);
  changeHumanLocation(1, 1, humans[1], grid);
  humans[0]->socialPreference = 0;
  humans[1]->socialPreference = 0;

  // humans[0] should not move to [1][1] ([1][1] is occupied)
  moveAStar(grid, humans, population, socialPlaces, gridLength, gridHeight, quarantine);

  testbool(!(humans[0]->x == humans[1]->x && humans[0]->y == humans[1]->y),
	   "Human cannot move to an occupied space");

  printf("\n");

  /* FREE VARIABLES */
  for (int i = 0; i < gridHeight; i++) {
    free(grid[i]);
  }

  if (numSocials) {
    free(socialPlaces);
  }
  
  free(grid);
  free(freeCells);

  for (int i = 0; i < population; i++) {
    free(humans[i]);
  }
	
  if(population > 0)
    free(humans);

  printf("Variables freed, end of tests\n");
}

