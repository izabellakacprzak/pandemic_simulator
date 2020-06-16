#ifndef SIMULATE_IO
#define SIMULATE_IO

#include "simulate_utils.h"
#define GRID_SIZE 10
#define BUFFER_SIZE (100)

#define DEFAULT_POPULATION (10)
#define DEFAULT_INFECTED (4)
#define LATENCY (5)
#define INF_CHANCE (0.4)
#define FATAL_CHANCE (0.1)
#define RECOVERY_CHANCE (0.6)

void printToTerminal(Grid grid, int gridLength, int gridHeight);

int getNextInput(char *input);

void setInitial(Disease *disease, int *population, int *initiallyInfected,int *gridLength, int *gridHeight);

void configurate(Disease *disease, int *population, int *initiallyInfected, int *gridLength, int *gridHeight);

void printConfigValues(Disease *disease, int *population, int *initiallyInfected, int *gridLength, int *gridHeight);

#endif
