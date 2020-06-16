#ifndef SIMULATE_SOCIAL
#define SIMULATE_SOCIAL

#include <stdlib.h>
#include "simulate_utils.h"

void initialiseSocials(int amount, Grid grid, SocialSpace *socialPlaces, int gridLength, int gridHeight);

void moveAStar(Grid grid, Human **humans, int population, SocialSpace *socialPlaces,
	  int length, int height);

#endif
