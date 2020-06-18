#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "simulationIO.h"

void printToTerminal(Grid grid, int gridColumns, int gridRows) {
  assert(gridColumns >= 0);
  assert(gridRows >= 0);

  GridCell cell;

  for(int i = 0; i < gridRows; i++) {
    for(int j = 0; j < gridColumns; j++) {
      cell = grid[i][j];
      if(cell.human) {
        switch (cell.human->status) {
          case HEALTHY:
            printf("H"); /* healthy */
            break;
          case LATENT:
            printf("L"); /* carrier */
            break;
          case SICK:
            printf("I"); /* infected */
            break;
          default:
            printf("E"); /* error */
            break;
        }
      } else {
        if(grid[i][j].type == SOCIAL) {
          printf("S"); /* social space */
        } else {
          printf("-"); /* empty cell */
        }
      }
    }
    printf("\n");
  }
}

int getNextInput(char *input) {
  printf("How many turns do you want to wait? (q to quit)\n");
  return scanf("%9s", input);
}

/* Sets an int variable with the value from the configuration file */
static void getInt(char *buffer, int *value) {
  if(!sscanf(buffer, "%*s%d", value)) {
    perror("Configuration value not found\n");
    exit(EXIT_FAILURE);
  }
}

/* Sets a double variable with the value from the configuration file */
static void getDouble(char *buffer, double *value){
  if(!sscanf(buffer, "%*s%lf", value)){
    perror("Configuration value not found\n");
    exit(EXIT_FAILURE);
  }
}

void setInitial(Disease *disease, int *population, int *initiallyInfected, int *gridColumns, int *gridRows, int *numSocials, int *quarantine){
  *population = DEFAULT_POPULATION;
  *initiallyInfected = DEFAULT_INFECTED;
  *gridColumns = GRID_SIZE;
  *gridRows = GRID_SIZE;
  *numSocials = SOCIAL_SPACES;
  disease->latencyPeriod = LATENCY;
  disease->infectionChance = INF_CHANCE;
  disease->fatalityChance = FATAL_CHANCE;
  disease->recoveryChance = RECOVERY_CHANCE;
  disease->immunity = 0;
  *quarantine = 0;  
}

void configure(Disease *disease, int *population, int *initiallyInfected, int *gridColumns, int *gridRows, int *numSocials, int *quarantine) {

  FILE *configFile;
  if((configFile = fopen("config.txt", "r")) == NULL) {
    printf("Error loading config.txt file... Reverting to default...\n");
  } else {
    char buffer[BUFFER_SIZE];

    while(!feof(configFile)) {
      fgets(buffer, BUFFER_SIZE, configFile);
      /* Comments in config file are denoted with / */
      if(buffer[0] == '/') {
        continue;
      } else if(strstr(buffer, "population")) {
        getInt(buffer, population);
      } else if(strstr(buffer, "initially_infected")) {
        getInt(buffer, initiallyInfected);
      } else if(strstr(buffer, "latency_period")) {
        getInt(buffer, &disease->latencyPeriod);
      } else if(strstr(buffer, "columns")) {
        getInt(buffer, gridColumns);
      } else if(strstr(buffer, "rows")) {
        getInt(buffer, gridRows);
      } else if(strstr(buffer, "infection_rate")) {
        getDouble(buffer, &disease->infectionChance);
      } else if(strstr(buffer, "fatality_rate")) {
        getDouble(buffer, &disease->fatalityChance);
      } else if(strstr(buffer, "recovery_rate")) {
        getDouble(buffer, &disease->recoveryChance);
      } else if(strstr(buffer, "immunity")) {
        getInt(buffer, &disease->immunity);
      } else if(strstr(buffer, "quarantine")) {
        getInt(buffer, quarantine);
      } else if(strstr(buffer, "social_spaces")) {
        getInt(buffer, numSocials);
      } else {
        printf("Configuration variable %s does not exist\n", buffer);
      }
    }
    fclose(configFile);
  }
  printConfigValues(disease, population, initiallyInfected, gridColumns, gridRows, numSocials, quarantine);
}

void printConfigValues(Disease *disease, int *population, int *initiallyInfected,
                       int *gridColumns, int *gridRows, int *numSocials, int *quarantine) {

  printf("\nPopulation is: %d\n", *population);
  printf("The number of initially infected is: %d\n", *initiallyInfected);
  printf("The latency period of the virus is: %d\n", disease->latencyPeriod);
  printf("The number of social spaces is %d\n", *numSocials);
  printf("The infection rate is: %lf\n", disease->infectionChance);
  printf("The fatality rate is: %lf\n", disease->fatalityChance);
  printf("The recovery rate is: %lf\n", disease->recoveryChance);
  printf("The immunity is %s\n", disease->immunity==1?"ON":"OFF");
  printf("the quarantine for sick humans is %s\n", *quarantine==1?"ON":"OFF");
  printf("The number of grid rows is: %d\n", *gridRows);
  printf("The number of grid columns is %d\n\n", *gridColumns);

}


