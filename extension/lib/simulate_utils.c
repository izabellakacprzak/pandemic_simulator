#include "simulate_utils.h"
#include "simulationIO.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
double randomFrom0To1(void) {
  return (double) rand() / (double) RAND_MAX;
}

void move(Grid grid, Human **humans, int population,
	  int length, int height) {
  int x, y;
  for(int i = 0; i < population; i++){
    if (humans[i]->status != DEAD) {
      do{
	x = humans[i]->x + (RANDINT(0, 3) - 1); //random int from -1 to 1
	y = humans[i]->y + (RANDINT(0, 3) - 1);
      } while(!(0 <= x && x < length) || !(0 <= y && y < height) ||
	      (grid[x][y].human && !(x == humans[i]->x && y == humans[i]->y)));
      CELL_CLEAR(grid[humans[i]->x][humans[i]->y]);
      humans[i]->x = x;
      humans[i]->y = y;
      CELL_SET(grid[humans[i]->x][humans[i]->y], humans[i]);
    }
  }
}

void checkInfections(Grid grid, Human **humans, int population,
		     int length, int height, Disease *disease) {
  int x, y;
  for (int i = 0; i < population; i++) {
    x = humans[i]->x;
    y = humans[i]->y;

    if (humans[i]->status == HEALTHY) {
      //check for neighbouring infected humans
      for (int j = x - 1; j <= x + 1; j++) {
	if (0 <= j && j < length) {
	  for (int k = y - 1; k <= y + 1; k++) {
	    if (0 <= k && k < height) {
	      //performs a random infection check for each infected neighbour
	      if (grid[j][k].human && grid[j][k].human->status != HEALTHY &&
		  randomFrom0To1() < disease->infectionChance) {
		humans[i]->status = LATENT;
		humans[i]->latencyTime = disease->latencyPeriod;
	      }
	    }
	  }
	}
      }
    }

    //checks to see whether symptoms start or recovery occurs
    if (humans[i]->status == LATENT) {
      if (humans[i]->latencyTime == 0) {
	//recovery or symptomous depends on human's risk and a random check
	if (randomFrom0To1() < humans[i]->risk) {
	  humans[i]->status = SICK;
	} else {
	  humans[i]->status = HEALTHY;
	}
      }
      //latency time reduced
      humans[i]->latencyTime--;
    }

    //checks for recovery or death
    if (humans[i]->status == SICK) {
      if (randomFrom0To1() < disease->fatalityChance) {
	humans[i]->status = DEAD;
	//remove from play
	CELL_CLEAR(grid[humans[i]->x][humans[i]->y]);
      } else if (randomFrom0To1() < disease->recoveryChance) {
	humans[i]->status = HEALTHY;
      }
    }
  }
}

char *getProgramError(ErrorCode e) {
  ErrorType errors[SYS];
  errors[ALLOCATION_FAIL].code = ALLOCATION_FAIL;
  errors[ALLOCATION_FAIL].message = "Out of Memory";
  

 return errors[e].message; 
}
