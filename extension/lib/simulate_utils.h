
#ifndef SIMULATE_UTILS
#define SIMULATE_UTILS

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
  //gridCell *cell;
  int risk; //could be a float based on design later on
  HealthStatus status;
} Human;

typedef struct gridCellStruct {
  Human *human;
  CellType type;
} GridCell;

typedef GridCell** Grid;

//sets a cell to be occupied by a given human
#define CELL_SET(cell, human) \
  do { cell.human = human; } while (0)

//sets a cell to be unoccupied
#define CELL_CLEAR(cell) \
  do { cell.human = NULL; } while (0)

#endif
