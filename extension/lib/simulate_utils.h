
#ifndef SIMULATE_UTILS
#define SIMULATE_UTILS

#include <stdlib.h>

typedef enum healthStatus {
  HEALTHY,
  LATENT,
  SICK
} HealthStatus;

typedef enum cellType {
  NORMAL,
  WORK,
  HOSPITAL
} CellType;

typedef struct humanStruct {
  int x;
  int y;
  double risk; 
  HealthStatus status;
} Human;

typedef struct gridCellStruct {
  Human *human;
  CellType type;
} GridCell;

typedef struct diseaseStruct {
  int latencyPeriod;
  double infectionChance;
  double fatalityChance;
} Disease;

typedef GridCell** Grid;

//sets a cell to be occupied by a given human
#define CELL_SET(cell, newHuman) \
  do { cell.human = newHuman; } while (0)

//sets a cell to be unoccupied
#define CELL_CLEAR(cell) \
  do { cell.human = NULL; } while (0)

#define RANDINT(min, max) \
  (rand() % (max - min)) + min

#define RAND_0_TO_1 \
  (rand() / 32767)

void move(Grid grid, Human **humans, Disease disease, int population,
	  int length, int height);
#endif
