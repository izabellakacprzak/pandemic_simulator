#include "simulate_utils.h"
#include "simulationIO.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define BUFFER_SIZE (100)

int main (int argc, char **argv) {
  char input[10];
  int noTurns;
  int gridLength = GRID_SIZE, gridWidth = GRID_SIZE;
  Grid grid = calloc(gridLength, sizeof(GridCell*));

  for (int i = 0; i < gridLength; i++) {
    grid[i] = calloc(gridWidth, sizeof(GridCell));
    //creates unoccupied cells of default type
  }

  srand(time(NULL));
  
  /* Declarations of variables */
  int population, initiallyInfected;
  Disease disease = {0};

  /* Open the config file and initalise the variables */
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
      if(!sscanf(buffer, "population=%d", &population)) {
	  perror("Configuration value not found\n");
	  exit(EXIT_FAILURE);
	}
    } else if(strstr(buffer, "initially_infected")) {
      if(!sscanf(buffer, "initially_infected=%d", &initiallyInfected)) {
	  perror("Configuration value not found\n");
	  exit(EXIT_FAILURE);
	}
    } else if(strstr(buffer, "latency_period")){
      if(!sscanf(buffer, "latency_period=%d", &(disease.latencyPeriod))){
	  perror("Configuration value not found\n");
	  exit(EXIT_FAILURE);
	}
    } else if(strstr(buffer, "infection_rate")){
      if(!sscanf(buffer, "infection_rate=%lf", &(disease.infectionChance))){
	  perror("Configuration value not found");
	  exit(EXIT_FAILURE);
	}
    } else if(strstr(buffer, "fatality_rate")){
      if(!sscanf(buffer, "fatality_rate=%lf", &(disease.fatalityChance))){
	  perror("Configuration value not found");
	  exit(EXIT_FAILURE);
	}
    } else {
      printf("configuration variable %s does not exist\n", buffer);
    }
  }
  fclose(configFile);

  printf("Population is: %d\n", population);
  printf("The number of initially infected is: %d\n", initiallyInfected);
  printf("The latency period of the virus is: %d\n", disease.latencyPeriod);
  printf("The infection rate is: %lf\n", disease.infectionChance);
  printf("The fatality rate is: %lf\n", disease.fatalityChance);   

  //creates an array of humans on the heap
  Human **humans = calloc(population, sizeof(Human*));

  int x, y;
  //TODO implement error handling of allocation fails
  for (int i = 0; i < population; i++) {
    humans[i] = calloc(1,sizeof(Human));
    do{
      x = RANDINT(0, gridLength - 1); 
      y = RANDINT(0, gridWidth - 1);
    } while (grid[x][y].human);
    //makes sure two humans cant be in the same square

    humans[i]->x = x;
    humans[i]->y = y;
    humans[i]->risk = randomFrom0To1();
    CELL_SET(grid[x][y], humans[i]);
  }

  //sets an initial number of humans to be infected
  for(int i = 0; i < initiallyInfected; i++) {
    humans[i]->status = LATENT;
    humans[i]->latencyTime = disease.latencyPeriod;
  }

  getNextInput(input);
  
  while (strcmp(input, "q")) {
    noTurns = atoi(input);

    for (int i = 0; i < noTurns; i++) {
      //call turn function
      move(grid, humans, population, gridLength, gridWidth);
      checkInfections(grid, humans, &population, gridLength, gridWidth, &disease);
    }

    printToTerminal(grid, gridLength, gridWidth);

    getNextInput(input);
  }
  for (int i = 0; i < gridLength; i++) {
    free(grid[i]);
  }

  for (int i = 0; i < population; i++) {
    free(humans[i]);
  }

  free(grid);
  return EXIT_SUCCESS;
}
