#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <time.h>

#include "simulate_utils.h"
#include "simulationIO.h"

void executeRand(int executions) {
  assert(executions >= 0);

  while (executions) {
    rand();
    executions--;
  }
}

void resetHumans(int newPopulation, int *population, Human **humans,
		 Grid *grid, int gridLength, int gridHeight) {
  for (int i = 0; i < *population; i++) {
    free(humans[i]);
  }

  for (int i = 0; i < newPopulation; i++) {
    int x, y;
    
    humans[i] = calloc(1, sizeof(Human));
    do{
      x = RANDINT(0, gridLength - 1); 
      y = RANDINT(0, gridHeight - 1);
    } while (grid[0][x][y].human);
    //makes sure two humans cant be in the same square

    humans[i]->x = x;
    humans[i]->y = y;
    humans[i]->risk = randomFrom0To1();
    // cell.human = newHuman
    CELL_SET(grid[0][x][y], humans[i]);
  }

  *population = newPopulation;
}

static void testbool(bool condition, const char *testname) {
  printf("T %s: %s\n", testname, condition?"OK":"FAIL");
}


static void testint(int got, int expected, char *testname) {
  printf("T %s (expected=%d, got=%d): %s\n",
	  testname, expected, got, expected==got?"OK":"FAIL");
}

/*
static void testlong(long got, long expected, char *testname) {
  printf("T %s (expected=%ld, got=%ld): %s\n",
	  testname, expected, got, expected==got?"OK":"FAIL");
}

static void testdouble(double got, double expected, char *testname) {
  printf("T %s (expected=%g, got=%g): %s\n",
	  testname, expected, got, expected==got?"OK":"FAIL");
}

static void teststring(char *got, char *expected, char *testname) {
  printf("T %s (expected='%s', got='%s'): %s\n",
	  testname, expected, got, strcmp(expected,got)==0?"OK":"FAIL");
}
*/

int main(void) {
  bool ok;
  
  /* SET SEED TO RANDOM */
  srand(time(NULL));
  
  /* ORIGINAL CONFIGS */
  int population = 10;
  int initiallyInfected = 5;

  Disease disease = {0};
  disease.latencyPeriod = 7;
  disease.infectionChance = 0.5;
  disease.fatalityChance = 0.1;

  int gridLength = 10;
  int gridHeight = 10;

  /* PRINT CONFIGS */
  printf("Population is: %d\n", population);
  printf("The number of initially infected is: %d\n", initiallyInfected);
  printf("The latency period of the virus is: %d\n", disease.latencyPeriod);
  printf("The infection rate is: %lf\n", disease.infectionChance);
  printf("The fatality rate is: %lf\n", disease.fatalityChance);
  printf("The grid is of size (%i, %i)\n", gridLength, gridHeight);
  printf("\n");

  /* INITIALISE VARIABLES */
  Grid grid = calloc(gridLength, sizeof(GridCell *));

  for (int i = 0; i < gridLength; i++) {
    grid[i] = calloc(gridHeight, sizeof(GridCell));
  }
  
  Human **humans = calloc(population, sizeof(Human *));

  for (int i = 0; i < population; i++) {
    int x, y;
    
    humans[i] = calloc(1, sizeof(Human));
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

  /* INITIALISATION TESTS */
  printf("Initialisation Tests:\n");

  {
    ok = true;
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
  /* outputs of RANDINT(0, 3) - 1: 
       0, 0, -1, 0, 1, 0, 0, -1, -1, 0, 1, 0,
       1, 0, 1, 0, -1, -1, 0, 0, 1, 1 */

  /* Test: move(Grid grid, Human **humans, int population, int length, int height) */
  printf("Tests for move (simulate_utils.c):\n");

  // population = 1, reset rand
  resetHumans(1, &population, humans, &grid, gridLength, gridHeight);
  srand(1);

  executeRand(1);

  humans[0]->x = 1;
  humans[0]->y = 1;
  // randoms:
  // x should set to x + 0 = 1
  // y should set to y - 1 = 0
  move(grid, humans, population, gridLength, gridHeight);

  testbool(humans[0]->x == 1 && humans[0]->y == 0, "Correct human location change");
  testbool(!grid[1][1].human, "Previous grid cell cleared");
  testbool(grid[humans[0]->x][humans[0]->y].human == humans[0],
	   "Destination cell occupied by human");
  
  
  printf("\n");
  
  /* FREE VARIABLES */
  for (int i = 0; i < gridLength; i++) {
    free(grid[i]);
  }

  free(grid);

  for (int i = 0; i < population; i++) {
    free(humans[i]);
  }
  free(humans);
  
  printf("Variables freed, end of tests\n");
}
