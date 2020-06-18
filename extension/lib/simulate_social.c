#include <stdlib.h>
#include <limits.h>
#include "simulate_social.h"
#include "simulate_utils.h"

void initialiseSocials(int amount, Grid grid, SocialSpace *socialPlaces,
		       int gridColumns, int gridRows) {
  int x, y;
  for (int i = 0; i < amount; i++) {
    do {
      x = RANDINT(0, gridColumns);
      y = RANDINT(0, gridRows);
    } while (grid[y][x].type == SOCIAL);

    grid[y][x].type = SOCIAL;
    socialPlaces[i].x = x;
    socialPlaces[i].y = y;
  }
}

void moveAStar(Grid grid, Human **humans, int population, SocialSpace *socialPlaces,
		int gridColumns, int gridRows) {
  for (int i = 0; i < population; i++) {
    int heuristics[3][3];

    if (humans[i]->status != SICK) {

      for (int x = -1; x < 2; x++) {
        for (int y = -1; y < 2; y++) {

          if (((humans[i]->x + x) >= 0 && (humans[i]->y + y) >= 0
               && (humans[i]->x + x) < gridColumns && (humans[i]->y + y) < gridRows) &&
              (!grid[humans[i]->y + y][humans[i]->x + x].human || (x == 0 && y == 0))) {
            int a = ((humans[i]->x + x) - socialPlaces[humans[i]->socialPreference].x);
            int b = ((humans[i]->y + y) - socialPlaces[humans[i]->socialPreference].y);
            heuristics[y + 1][x + 1] = (a * a) + (b * b);

          } else {
            heuristics[y + 1][x + 1] = INT_MAX;
          }
        }
      }
      int minX = 0, minY = 0, minHeur = INT_MAX;
      for (int x = 0; x < 3; x++) {
        for (int y = 0; y < 3; y++) {
          if (heuristics[y][x] < minHeur) {
            minHeur = heuristics[y][x];
            minX = x - 1;
            minY = y - 1;
          }
        }
      }
      if (minHeur < INT_MAX) {
        cellClear(&grid[humans[i]->y][humans[i]->x]);
        humans[i]->x += minX;
        humans[i]->y += minY;
        cellSet(&grid[humans[i]->y][humans[i]->x], humans[i]);
      }
    }
  }
}
