#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

#include "simulate_social.h"
#include "simulate_utils.h"
#include "simulationIO.h"

void initialiseSocials(int amount, Grid grid, SocialSpace *socialPlaces,
		       int gridLength, int gridHeight) {
  int x, y;
  for (int i = 1; i <= amount; i++) {
    do {
      x = RANDINT(0, gridLength - 1);
      y = RANDINT(0, gridHeight - 1);
    } while (grid[x][y].type == SOCIAL);

    grid[x][y].type = SOCIAL;
    socialPlaces[i - 1].x = x;
    socialPlaces[i - 1].y = y;
  }

}

static int inBounds(int length, int height, int x, int y){
  return (x >= 0 && y >= 0 && x < length && y < height);
}

void moveAStar(Grid grid, Human **humans, int population, SocialSpace *socialPlaces,
		int length, int height) {
  for(int i = 0; i < population; i++){
    int heuristics[3][3] = {0};
    if(humans[i]->status != DEAD) {
      for(int x = -1; x < 2; x++) {
	for(int y = -1; y < 2; y++) {
	  if(inBounds(length, height, humans[i]->x + x, humans[i]->y + y) &&
	     (grid[humans[i]->x + x][humans[i]->y +y].human && (x != 0 && y != 0))){
	    int a = ((humans[i]->x + x) - socialPlaces[humans[i]->socialPreference - 1].x);
	    int b = ((humans[i]->y + y) - socialPlaces[humans[i]->socialPreference - 1].y);
	    heuristics[x][y] = a*a + b*b;
	  } else{
	    heuristics[x][y] = INT_MAX;
	  }
	}
      }
    }
    int minX = 0;
    int minY = 0;
    int minHeur = INT_MAX;
        
    for(int x = 0; x < 3; x++) {
      for(int y = 0; y < 3; y++) {
	if (heuristics[x][y] < minHeur){
	  minHeur = heuristics[x][y];
	  minX = x - 1;
	  minY = y - 1;
	}
      }
    }
    CELL_CLEAR(grid[humans[i]->x][humans[i]->y]);
    humans[i]->x += minX;
    humans[i]->y += minY;
    CELL_SET(grid[humans[i]->x][humans[i]->y], humans[i]);
  }
}
