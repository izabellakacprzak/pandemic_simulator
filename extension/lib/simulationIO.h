#ifndef SIMULATE_IO
#define SIMULATE_IO

#include "simulate_utils.h"
#define GRID_SIZE (10)
#define BUFFER_SIZE (100)

#define DEFAULT_POPULATION (10)
#define DEFAULT_INFECTED (4)
#define LATENCY (5)
#define INF_CHANCE (0.4)
#define FATAL_CHANCE (0.1)
#define RECOVERY_CHANCE (0.6)
#define SOCIAL_SPACES (2)

void printToTerminal(Grid grid, int gridColumns, int gridRows);

int getNextInput(char *input);

void setInitial(Disease *disease, int *population, int *initiallyInfected,
        int *gridColumns, int *gridRows, int *numSocials, int *quarantine);

void configurate(Disease *disease, int *population, int *initiallyInfected,
        int *gridColumns, int *gridRows, int *numSocials, int *quarantine);

void printConfigValues(Disease *disease, int *population, int *initiallyInfected,
                       int *gridColumns, int *gridRows, int *numSocials, int *quarantine);

#endif
