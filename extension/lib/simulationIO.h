#ifndef SIMULATE_IO
#define SIMULATE_IO

#include "simulate_utils.h"

#define BUFFER_SIZE (100)

/* Default values used instead of the configure file */
#define GRID_SIZE (100)
#define DEFAULT_POPULATION (500)
#define DEFAULT_INFECTED (100)
#define LATENCY (7)
#define INF_CHANCE (0.8)
#define FATAL_CHANCE (0.05)
#define RECOVERY_CHANCE (0.05)
#define SOCIAL_SPACES (5)
#define IMMUNITY (1)
#define QUARANTINE (0)

/* Prints a grid to the terminal with the social places 
   and the status of each human displayed accordingly */
void printToTerminal(Grid grid, int gridColumns, int gridRows);

/* Gets the number of turns requested by the user */
int getNextInput(char *input);

/* Sets the default values of the varibales */
void setInitial(Disease *disease, int *population, int *initiallyInfected,
        int *gridColumns, int *gridRows, int *numSocials, int *quarantine);

/* Sets the values of the variables accordingly to 
   the ones chosen by the user in the config.txt file */
void configure(Disease *disease, int *population, int *initiallyInfected,
        int *gridColumns, int *gridRows, int *numSocials, int *quarantine);

/* Prints the variables */
void printConfigValues(Disease *disease, int *population, int *initiallyInfected,
                       int *gridColumns, int *gridRows, int *numSocials, int *quarantine);

#endif
