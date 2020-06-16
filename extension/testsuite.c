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

	for (int i = 0; i < gridLength; i++) {
		for (int j = 0; j < gridHeight; j++) {
			grid[0][i][j].human = NULL;
		}
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

void resetDisease(Disease *disease) {
	disease->latencyPeriod = 0;
	disease->infectionChance = 0;
	disease->fatalityChance = 0;
	disease->recoveryChance = 0;
}

void changeHumanLocation(int x, int y, Human *human, Grid *grid) {
	grid[0][human->x][human->y].human = NULL;
	grid[0][x][y].human = human;
	human->x = x;
	human->y = y;
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

	int gridLength = 10; // Note: must be >= 3
	int gridHeight = 10; // Note: must be >= 3

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
	printf("SIMULATE_UTILS.C:\n");

	/* Test: move(Grid grid, Human **humans, int population,
	   int length, int height) */
	printf("Tests for move (simulate_utils.c):\n");

	// population = 1, reset rand
	resetHumans(1, &population, humans, &grid, gridLength, gridHeight);
	srand(1);

	executeRand(1);

	changeHumanLocation(1, 1, humans[0], &grid);
	// randoms: {0, -1}
	// x should set to x + 0 = 1
	// y should set to y - 1 = 0
	move(grid, humans, population, gridLength, gridHeight);

	testbool(humans[0]->x == 1 && humans[0]->y == 0, "Correct human location change");
	testbool(!grid[1][1].human, "Previous grid cell cleared");
	testbool(grid[humans[0]->x][humans[0]->y].human == humans[0],
			"Destination cell occupied by the right human");

	executeRand(4);

	changeHumanLocation(0, 0, humans[0], &grid);
	// randoms: {-1, -1, 0, 1}
	// {-1, -1} should be ignored
	// x should set to x + 0 = 0
	// y should set to y + 1 = 1
	move(grid, humans, population, gridLength, gridHeight);

	testbool(humans[0]->x >= 0 && humans[0]->y >= 0,
			"Human cannot be placed outside of grid (left and top)");

	executeRand(9);

	changeHumanLocation(gridLength - 1, gridHeight - 1, humans[0], &grid);
	// randoms: {1, 1, -1, -1}
	// {1, 1} should be ignored
	// x should set to x - 1 = gridLength - 1
	// y should set to y - 1 = gridHeight - 1
	move(grid, humans, population, gridLength, gridHeight);

	testbool(humans[0]->x < gridLength && humans[0]->y < gridHeight,
			"Human cannot be placed outside of grid (right and bottom)");

	resetHumans(2, &population, humans, &grid, gridLength, gridHeight);
	changeHumanLocation(1, 1, humans[0], &grid);
	changeHumanLocation(2, 0, humans[1], &grid);

	// randoms: {1, -1, -1, -1, 1, -1, 0, 0, 0, 0}
	// {1, -1} should be ignored by humans[0] ([2][0] is occupied)
	// humans[0]->x should be x - 1 = 0
	// humans[0]->y should be y - 1 = 0
	move(grid, humans, population, gridLength, gridHeight);

	testbool(!(humans[0]->x == humans[1]->x && humans[0]->y == humans[1]->y),
			"Human cannot move to an occupied space");

	printf("\n");

	/* Test: checkInfections(Grid grid, Human **humans, int population,
	   int length, int height, Disease *disease) */
	printf("Tests for checkInfections (simulate_utils.c):\n");

	srand(1);

	/* using randomFrom1To0:
randoms: {0.840188, 0.394383, 0.783099, 0.798440, 0.911647,
0.197551, 0.335223, 0.768230, 0.277775, 0.553970,
0.477397, 0.628871, 0.364784, 0.513401, 0.952230,
0.916195, 0.635712, 0.717297, 0.141603, 0.606969,
0.016301, 0.242887, 0.137232, 0.804177, 0.156679,
0.400944, 0.129790, 0.108809, 0.998925, 0.218257,
0.512932, 0.839112, 0.612640, 0.296032, 0.637552,
0.524287, 0.493583, 0.972775, 0.292517, 0.771358} */

	resetHumans(1, &population, humans, &grid, gridLength, gridHeight);
	resetDisease(&disease);
	disease.infectionChance = 1;
	disease.latencyPeriod = 10;
	// fatalityChance and recoveryChance = 0
	humans[0]->status = HEALTHY;

	checkInfections(grid, humans, &population, gridLength, gridHeight, &disease);

	testbool(humans[0]->status == HEALTHY, "Single healthy human remains healthy");

	resetHumans(9, &population, humans, &grid, gridLength, gridHeight);
	for (int i = 0; i < population; i++) {
		changeHumanLocation(i % 3, i / 3, humans[i], &grid);
		humans[i]->risk = 0;
		humans[i]->latencyTime = 10;
		humans[i]->status = HEALTHY;
	}
	/* Visual representation:
	   0 1 2 -
	   3 4 5 - (rest of grid)
	   6 7 8 - 
	   - - -   */

	humans[4]->status = LATENT;
	checkInfections(grid, humans, &population, gridLength, gridHeight, &disease);

	{
		int latent = 0;
		for (int i = 0; i < population; i++) {
			if (i != 4 && humans[i]->status == LATENT) {
				latent++;
			}
		}

		testint(latent, 8, "Adjacent humans of a LATENT human are made LATENT [risk = 0]");
	}


	for (int i = 0; i < population; i++) {
		humans[i]->risk = 0;
		humans[i]->latencyTime = 10;
		humans[i]->status = HEALTHY;
	}

	humans[8]->status = LATENT;

	checkInfections(grid, humans, &population, gridLength, gridHeight, &disease);

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

	resetHumans(2, &population, humans, &grid, gridLength, gridHeight);
	changeHumanLocation(1, 1, humans[0], &grid);
	changeHumanLocation(2, 1, humans[1], &grid);
	humans[0]->status = SICK;
	humans[1]->status = HEALTHY;
	disease.infectionChance = 1;
	checkInfections(grid, humans, &population, gridLength, gridHeight, &disease);

	testbool(humans[1]->status == LATENT, "When latencyChance = 1 all humans who have contant with sick humans become LATENT");

	{
		resetHumans(10, &population, humans, &grid, gridLength, gridHeight);
		disease.fatalityChance = 1;
		for(int i = 0; i < population; i++){
			humans[i]->status = SICK;
		}

		checkInfections(grid, humans, &population, gridLength, gridHeight, &disease);

		testint(population, 0, "When infectionChance is 1 all infected humans should die");
	}
	printf("\n");

	/* FREE VARIABLES */
	for (int i = 0; i < gridLength; i++) {
		free(grid[i]);
	}

	free(grid);

	for (int i = 0; i < population; i++) {
		free(humans[i]);
	}
	
	if(population > 0)
		free(humans);

	printf("Variables freed, end of tests\n");
}
