#ifndef SIMULATE_IO
#define SIMULATE_IO

#include "simulate_utils.h"
#define GRID_SIZE 10

void printToTerminal(Grid grid, int gridLength, int gridHeight);

int getNextInput(char *input);

void configurate(Disease *disease, int *population, int *initiallyInfected);

void printConfigValues(Disease *disease, int *population, int *initiallyInfected);

#endif
