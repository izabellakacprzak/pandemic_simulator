#include <stdlib.h>
#include <stdio.h>

#include "simulate_social.h"
#include "simulate_utils.h"
#include "simulationIO.h"

void initialiseSocials(int amount, Grid grid, GridCell *socialPlaces,
		       int gridLength, int gridHeight) {
  int x, y;
  for (int i = 1; i <= amount; i++) {
    do {
      x = RANDINT(0, gridLength - 1);
      y = RANDINT(0, gridHeight - 1);
    } while (grid[x][y].type == SOCIAL);

    grid[x][y].type = SOCIAL;
    grid[x][y].spaceNumber = i;
    socialPlaces[i - 1] = grid[x][y];
  }

}

/*
void moveAStar (Grid grid, Human **humans, int population,
	  int length, int height) {
  int x, y;
  for(int i = 0; i < population; i++){
    if(humans[i]->status != DEAD) {
      
    }

  }


  
}

*/
