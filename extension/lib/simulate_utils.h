
#ifndef SIMULATE_UTILS
#define SIMULATE_UTILS

#include <stdlib.h>

typedef enum healthStatus {
  HEALTHY,
  LATENT,
  SICK,
  DEAD
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
  int latencyTime;
} Human;

typedef struct gridCellStruct {
  Human *human;
  CellType type;
} GridCell;

typedef struct diseaseStruct {
  int latencyPeriod;
  double infectionChance;
  double fatalityChance;
  double recoveryChance;
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

double randomFrom0To1(void);

void move(Grid grid, Human **humans, int population, int length, int height);

void checkInfections(Grid grid, Human **humans, int *population, int length, int height, Disease *disease);
#endif
