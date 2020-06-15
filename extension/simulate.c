#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE (100)

/*
void readInt(int *val, char *buffer){
  char *string = calloc(1, sizeof(char));
  if(!sscanf(buffer, "%s=%d", string, val)) {
    perror("Configuration value not found\n");
    exit(EXIT_FAILURE);
  }
  free(string);
  } */

int main (int argc, char **argv) {

  /* Declarations of variables */
  int population, initiallyInfected, latencyPeriod;
  double infectionRate, fatalityRate;

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
      if(!sscanf(buffer, "latency_period=%d", &latencyPeriod)){
	  perror("Configuration value not found\n");
	  exit(EXIT_FAILURE);
	}
    } else if(strstr(buffer, "infection_rate")){
      if(!sscanf(buffer, "infection_rate=%lf", &infectionRate)){
	  perror("Configuration value not found");
	  exit(EXIT_FAILURE);
	}
    } else if(strstr(buffer, "fatality_rate")){
      if(!sscanf(buffer, "fatality_rate=%lf", &fatalityRate)){
	  perror("Configuration value not found");
	  exit(EXIT_FAILURE);
	}
    }
  }
  fclose(configFile);

  printf("Population is: %d\n", population);
  printf("The number of initially infected is: %d\n", initiallyInfected);
  printf("The latency period of the virus is: %d\n", latencyPeriod);
  printf("The infection rate is: %lf\n", infectionRate);
  printf("The fatality rate is: %lf\n", fatalityRate);   

  return EXIT_SUCCESS;
}
