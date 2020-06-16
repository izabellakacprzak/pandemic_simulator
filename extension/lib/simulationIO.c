#include "simulationIO.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>

#define BUFFER_SIZE (100)

void printToTerminal(Grid grid, int gridLength, int gridHeight) {
  assert(gridLength >= 0);
  assert(gridHeight >= 0);

  GridCell cell;

  for (int i = 0; i < gridHeight; i++) {
    for (int j = 0; j < gridLength; j++) {
      cell = grid[j][i]; //assuming array goes [x][y]
      if (cell.human) {
	switch (cell.human->status) {
	case HEALTHY:
	  printf("H"); //healthy
	  break;
	case LATENT:
	  printf("L"); //carrier
	  break;
	case SICK:
	  printf("I"); //infected
	  break;
	default:
	  printf("E"); //error
	  break;
	}
      } else {
	printf("-"); //empty cell
      }
    }
    printf("\n");
  }
}

int getNextInput(char *input) {
  printf("How many turns do you want to wait? (q to quit)\n");
  return scanf("%9s", input);
}

static void getInt(char *buffer, int *value){
  if(!sscanf(buffer, "%*s%d", value)){
    perror("Configuration value not found\n");
    exit(EXIT_FAILURE);
  }
}

static void getDouble(char *buffer, double *value){
  if(!sscanf(buffer, "%*s%lf", value)){
    perror("Configuration value not found\n");
    exit(EXIT_FAILURE);
  }
}

void configurate(Disease *disease, int *population, int *initiallyInfected) {

  FILE *configFile;
    if((configFile = fopen("config.txt", "r" )) == NULL){
        perror("Error loading configuration file");
	exit(EXIT_FAILURE);
    }

  char buffer[BUFFER_SIZE];

  //Code duplication to remove - this is a working version
  while(!feof(configFile)) {
  
    fgets(buffer, BUFFER_SIZE, configFile);
    /* Comments in config file are denoted with / */
    if(buffer[0] == '/') {
      continue;
    } else if(strstr(buffer, "population")) {
      getInt(buffer, population);
    } else if(strstr(buffer, "initially_infected")) {
      getInt(buffer, initiallyInfected);
    } else if(strstr(buffer, "latency_period")){
      getInt(buffer, &disease->latencyPeriod);      
    } else if(strstr(buffer, "infection_rate")){
      getDouble(buffer, &disease->infectionChance);
    } else if(strstr(buffer, "fatality_rate")){
      getDouble(buffer, &disease->fatalityChance);
    } else {
      printf("Configuration variable %s does not exist\n", buffer);
    }
  }
  fclose(configFile);

  printConfigValues(disease, population, initiallyInfected); 

}

void printConfigValues(Disease *disease, int *population, int *initiallyInfected) {

  printf("Population is: %d\n", *population);
  printf("The number of initially infected is: %d\n", *initiallyInfected);
  printf("The latency period of the virus is: %d\n", disease->latencyPeriod);
  printf("The infection rate is: %lf\n", disease->infectionChance);
  printf("The fatality rate is: %lf\n", disease->fatalityChance);   

}
