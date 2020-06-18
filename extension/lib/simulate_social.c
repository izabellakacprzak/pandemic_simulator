#include <stdlib.h>
#include <limits.h>
#include "simulate_social.h"
#include "simulate_utils.h"

void initialiseSocials(int amount, Grid grid, SocialSpace *socialPlaces,
		       int gridColumns, int gridRows) {
  int x, y;
  /* Find a normal type cell and change its type to social */
  for(int i = 0; i < amount; i++) {
    do {
      x = RANDINT(0, gridColumns);
      y = RANDINT(0, gridRows);
    } while (grid[y][x].type == SOCIAL);

    /* Include it into the array of socialPlaces by 
       noting down its coordinates */
    grid[y][x].type = SOCIAL;
    socialPlaces[i].x = x;
    socialPlaces[i].y = y;

    /* Assign the spaces around the social space to type
       NEAR_SOCIAL (drawn in the same colour as SOCIAL) */
    int radius = (gridColumns * gridRows) / (50 * 50);
    for(int i = -radius + x; i < radius + x; i++) {
      for(int j = -radius + y; j < radius + y; j++) {
	if(i < gridColumns && j < gridRows && i >= 0 && j >=0) {
	  grid[j][i].type = NEAR_SOCIAL;
	}
      }
    }
    
  }
}

void moveAStar(Grid grid, Human **humans, int population, SocialSpace *socialPlaces,
		int gridColumns, int gridRows, int quarantine) {
   for(int i = 0; i < population; i++) {
    /* An array to be populated with the heuristics of the 9 cells 
       surrounding a human, including the one he is standing on */
    int heuristics[3][3];
    if(!(quarantine && humans[i]->status == SICK)) {
    
      for(int x = -1; x < 2; x++) {
        for(int y = -1; y < 2; y++) {
        /* Calculate the heuristic if the cell isn't out of bounds 
           and isn't taken by another human; else set it to INT_MAX */
          if (((humans[i]->x + x) >= 0 && (humans[i]->y + y) >= 0
               && (humans[i]->x + x) < gridColumns && (humans[i]->y + y) < gridRows) &&
              (!grid[humans[i]->y + y][humans[i]->x + x].human || (x == 0 && y == 0))) {
              /* When calculating the heuristic, take into consideration the prefered place */
            int a = ((humans[i]->x + x) - socialPlaces[humans[i]->socialPreference].x);
            int b = ((humans[i]->y + y) - socialPlaces[humans[i]->socialPreference].y);
            heuristics[y + 1][x + 1] = (a * a) + (b * b);
          } else {
            heuristics[y + 1][x + 1] = INT_MAX;
          }
        }
      }
      /* Find the minimal heuristic */
      int minX = 0, minY = 0, minHeur = INT_MAX;
      for(int x = 0; x < 3; x++) {
        for(int y = 0; y < 3; y++) {
          if(heuristics[y][x] < minHeur) {
            minHeur = heuristics[y][x];
            minX = x - 1;
            minY = y - 1;
          }
        }
      }
      /* Move the human to the cell with the minimal heuristic;
         if there is no minimal heuristic - don't move */
      if(minHeur < INT_MAX) {
        cellClear(&grid[humans[i]->y][humans[i]->x]);
        humans[i]->x += minX;
        humans[i]->y += minY;
        cellSet(&grid[humans[i]->y][humans[i]->x], humans[i]);
      }
    }
  }
}
