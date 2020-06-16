#include "simulate_utils.h"
#include "simulationIO.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>



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

  configurate(&disease, &population, &initiallyInfected);

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
      checkInfections(grid, humans, population, gridLength, gridWidth, &disease);
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
