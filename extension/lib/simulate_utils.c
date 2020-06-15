#include "simulate_utils.h"
#include "simulationIO.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void move(Grid grid, Human **humans, Disease disease, int population,
	  int length, int height) {
  printf("Starting move\n");
  int x, y;
  for(int i = 0; i < population; i++){
    printf("%d\n", i);
    do{
      x = humans[i]->x + (RANDINT(0, 3) - 1);
      y = humans[i]->y + (RANDINT(0, 3) - 1);
      printf("x = %d, y = %d\n", x, y);
    } while(!(0 <= x && x < length) || !(0 <= y && y < height) ||
	    (grid[x][y].human && !(x == humans[i]->x && y == humans[i]->y)));
    CELL_CLEAR(grid[humans[i]->x][humans[i]->y]);
    humans[i]->x = x;
    humans[i]->y = y;
    CELL_SET(grid[humans[i]->x][humans[i]->y], humans[i]);
    printf("Ended moving %d\n", i);
  }
}

