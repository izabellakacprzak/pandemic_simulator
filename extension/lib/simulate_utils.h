
#ifndef SIMULATE_UTILS
#define SIMULATE_UTILS

#include <stdlib.h>
#include <errno.h>

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

typedef enum errorCode {
  OK,
  ALLOCATION_FAIL,
  SYS
} ErrorCode;

/* Struct containing the error message based on the error code*/
typedef struct err {
  int code;
  char *message;
} ErrorType;

#define EC_FROM_SYS_ERROR(e) (SYS + e)
#define EC_TO_SYS_ERROR(e) (e - SYS)
#define EC_IS_SYS_ERROR(e) (e >= SYS)

/* For program errors such as invalid instructions */
#define FATAL_PROG(pred, status) \
  do { if (pred) {err = status; goto fatalError;} } while (0)

/* For system errors such as failed file open */
#define FATAL_SYS(pred) \
  do { if (pred) {err = EC_FROM_SYS_ERROR(errno); goto fatalError;} } while (0)

char *getProgramError(ErrorCode e);
#endif