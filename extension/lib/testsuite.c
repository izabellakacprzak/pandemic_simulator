#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#include "simulate_utils.h"
#include "simulationIO.h"

void executeRand(int executions) {
  assert(executions >= 0);

  while (executions) {
    rand();
    executions--;
  }
}

static void testbool(bool condition, char *testname) {
  printf("T %s: %s\n", testname, condition?"OK":"FAIL");
}

/*
static void testint(int got, int expected, char *testname) {
  printf("T %s (expected=%d, got=%d): %s\n",
	  testname, expected, got, expected==got?"OK":"FAIL");
}

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
  /* SET SEED */
  srand(1);
  
  /* ORIGINAL CONFIGS */
  double population = 10;
  double initiallyInfected = 5;

  Disease disease = {0};
  disease.latencyPeriod = 7;
  disease.infectionChance = 0.5;
  disease.fatalityChance = 0.1;

  int gridLength = 10;
  int gridHeight = 10;

  /* INITIALISE VARIABLES */
  Human **humans = calloc(population, sizeof(Human *));
  Grid grid = calloc(gridLength, sizeof(GridCell *));

  for (int i = 0; i < gridLength; i++) {
    grid[i] = calloc(gridHeight, sizeof(GridCell));
  }

  /* PRINT VARIABLES */
  printf("Population is: %f\n", population);
  printf("The number of initially infected is: %f\n", initiallyInfected);
  printf("The latency period of the virus is: %d\n", disease.latencyPeriod);
  printf("The infection rate is: %lf\n", disease.infectionChance);
  printf("The fatality rate is: %lf\n", disease.fatalityChance);
  printf("The grid is of size (%i, %i)\n", gridLength, gridHeight);

  /* SIMULATE_UTILS.C TESTS */
  /* outputs of RANDINT(0, 3) - 1: 
       0, 0, -1, 0, 1, 0, 0, -1, -1, 0, 1, 0,
       1, 0, 1, 0, -1, -1, 0, 0, 1, 1 */

  // move(Grid grid, Human **humans, int population, int length, int height)
  
  /* FREE VARIABLES */
  for (int i = 0; i < gridLength; i++) {
    free(grid[i]);
  }

  free(grid);
  free(humans);
}
