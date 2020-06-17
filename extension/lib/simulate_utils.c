#include "simulate_utils.h"
#include "simulationIO.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
double randomFrom0To1(void) {
  return (double) rand() / (double) RAND_MAX;
}

void cellSet(GridCell *cell, Human *newHuman) {
  cell->human = newHuman;
}

void cellClear(GridCell *cell) {
  cell->human = NULL;
}

void move(Grid grid, Human **humans, int population,
          int gridColumns, int gridRows) {
  int x, y;
  for(int i = 0; i < population; i++) {
    if(humans[i]->status != SICK) {
      do{
	x = humans[i]->x + (RANDINT(0, 3) - 1); //random int from -1 to 1
	y = humans[i]->y + (RANDINT(0, 3) - 1);
      } while((0 > x || x >= gridColumns || 0 > y || y >= gridRows) ||
              (grid[y][x].human && grid[y][x].human != humans[i]
		/*&& !(x == humans[i]->x && y == humans[i]->y)*/));
      cellClear(&grid[humans[i]->y][humans[i]->x]);
      humans[i]->x = x;
      humans[i]->y = y;
      cellSet(&grid[humans[i]->y][humans[i]->x], humans[i]);
    }
  }
}

static HealthStatus **statusGrid(Human **humans, int population, int gridColumns, int gridRows) {
  HealthStatus **prevState = calloc(gridRows, sizeof(HealthStatus *));

  for(int i = 0; i < gridRows; i++) {
    prevState[i] = calloc(gridColumns, sizeof(HealthStatus));
  }

  for(int i = 0; i < population; i++) {
    prevState[humans[i]->y][humans[i]->x] = humans[i]->status;
  }

  return prevState;
}

static void freeStatusGrid(HealthStatus **state, int gridRows) {
  if (!state) {
    return;
  }
  
  for (int i = 0; i < gridRows; i++) {
    if (state[i]) {
      free(state[i]);
    }
  }

  free(state);
}

void checkInfections(Grid grid, Human **humans, int *population,
                     int gridColumns, int gridRows, Disease *disease) {

    HealthStatus **prevState = statusGrid(humans, *population, gridColumns, gridRows);

    int x, y;
    for(int i = 0; i < *population; i++) {
        x = humans[i]->x;
        y = humans[i]->y;

        if(humans[i]->status == HEALTHY) {
            //check for neighbouring infected humans
            for (int j = x - 1; j <= x + 1; j++) {
                if (0 <= j && j < gridColumns) {
                    for (int k = y - 1; k <= y + 1; k++) {
                        if (0 <= k && k < gridRows) {
                            //performs a random infection check for each infected neighbour
                            if (prevState[k][j] != HEALTHY &&
                                randomFrom0To1() < disease->infectionChance
				&& randomFrom0To1() < humans[i]->risk) {
                                humans[i]->status = LATENT;
                                humans[i]->latencyTime = disease->latencyPeriod;
                            }
                        }
                    }
                }
            }
        }

        //checks to see whether symptoms start or recovery occurs
        if(humans[i]->status == LATENT) {
            if(humans[i]->latencyTime == 0) {
                //recovery or symptomous depends on human's risk and a random check
                if (randomFrom0To1() < humans[i]->risk) {
                    humans[i]->status = SICK;
                } else {
		    humans[i]->risk = 0;
                    humans[i]->status = HEALTHY;
                }
            }
            //latency time reduced
            humans[i]->latencyTime--;
        }

        //checks for recovery or death
        if(humans[i]->status == SICK) {
            if(randomFrom0To1() < disease->fatalityChance) {
                humans[i]->status = DEAD;

                //remove from play
                cellClear(&grid[humans[i]->y][humans[i]->x]);
                if (i < *population - 1) {
                    //humans[i] = humans[*population - 1];
                    //memmove(&humans[i], &humans[i+1], sizeof(Human *) * (*population - i - 1));
                    *humans[i] = *humans[*population - 1];
                    cellSet(&grid[humans[i]->y][humans[i]->x], humans[i]);
                }
                if(humans[*population - 1]) {
                    free(humans[*population - 1]);
                    humans[*population - 1] = NULL;
                }
                *population = *population - 1;
                i--;
            } else if(randomFrom0To1() < disease->recoveryChance) {
                humans[i]->risk = 0;
		humans[i]->status = HEALTHY;
            }
        }
    }

    freeStatusGrid(prevState, gridRows);
}

char *getProgramError(ErrorCode e) {
  ErrorType errors[SYS];
  errors[ALLOCATION_FAIL].code = ALLOCATION_FAIL;
  errors[ALLOCATION_FAIL].message = "Out of Memory";
  

 return errors[e].message; 
}
