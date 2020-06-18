#ifndef SIMULATE_SOCIAL
#define SIMULATE_SOCIAL

#include <stdlib.h>
#include "simulate_utils.h"

void initialiseSocials(int amount, Grid grid, SocialSpace *socialPlaces, int gridColumns, int gridRows);

void moveAStar(Grid grid, Human **humans, int population, SocialSpace *socialPlaces,
	  int gridColumns, int gridRows, int quarantine);

#endif
