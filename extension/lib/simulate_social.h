#ifndef SIMULATE_SOCIAL
#define SIMULATE_SOCIAL

#include <stdlib.h>
#include "simulate_utils.h"

/* Initialises social places on the grid and adds them to the array of social places */
void initialiseSocials(int amount, Grid grid, SocialSpace *socialPlaces, int gridColumns, int gridRows);

/* Implements a simplified A* algorithm which calculates and 
   moves each human towards their prefered social place if possible */
void moveAStar(Grid grid, Human **humans, int population, SocialSpace *socialPlaces,
	  int gridColumns, int gridRows);

#endif
