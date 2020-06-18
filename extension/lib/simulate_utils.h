#ifndef SIMULATE_UTILS
#define SIMULATE_UTILS

#include <stdlib.h>
#include <errno.h>

#define EC_FROM_SYS_ERROR(e) (SYS + e)
#define EC_TO_SYS_ERROR(e) (e - SYS)
#define EC_IS_SYS_ERROR(e) (e >= SYS)

/* For program errors such as invalid instructions */
#define FATAL_PROG(pred, status) \
  do { if (pred) {err = status; goto fatalError;} } while (0)

/* For system errors such as failed file open */
#define FATAL_SYS(pred) \
  do { if (pred) {err = EC_FROM_SYS_ERROR(errno); goto fatalError;} } while (0)

/* Enum which displays the health status of a human*/
typedef enum healthStatus {
  HEALTHY,
  LATENT,
  SICK,
  DEAD
} HealthStatus;

/* Enum which displays whether the cell is a social place or not */
typedef enum cellType {
  NORMAL,
  SOCIAL,
} CellType;

/* Struct for holding the coordinates of a point */
typedef struct point {
	int x;
	int y;
} Point;

/* Typedef for social spaces which hold the 
   coordinates of a social space on the grid */
typedef Point SocialSpace;

/* Struct which holds the coordinates of the human, their prefered social space,
   the time they have been latent for and their risk of catching the virus */
typedef struct humanStruct {
  int latencyTime;
  int x;
  int y;
  int socialPreference;
  double risk;
  HealthStatus status;
} Human;

/* Struct for the grid cells with each holding a pointer to 
   the human currently in the cell as well as the type of the cell */
typedef struct gridCellStruct {
  Human *human;
  CellType type;
} GridCell;

/* Typedef for a 2d array made out of grid cells */
typedef GridCell** Grid;

typedef struct diseaseStruct {
  int latencyPeriod;
  double infectionChance;
  double fatalityChance;
  double recoveryChance;
} Disease;

/* Enum which displays the output selected by the user */
typedef enum outputSelection {
  NO_OUTPUT,
  GIF,
  TERMINAL
} outputSelection;

/* Enum which displays the configuration selected by the user */
typedef enum configSelection {
  NO_CONFIGURATION,
  CONFIG
} configSelection;


/* Enum containing the error codes performed by the program */
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

#define RANDINT(min, max) \
  (rand() % (max - min)) + min

/* Returns a random double between 0 and 1 with the random generator seeded in main */
double randomFrom0To1(void);

/* Assigns a cell a new human */
void cellSet(GridCell *cell, Human *newHuman);

/* Clears the Human field of a cell */
void cellClear(GridCell *cell);

/* Impements a random movement algorith which moves 
   each human to a random free cell next to them */
void move(Grid grid, Human **humans, int population, int gridColumns, int gridRows);

/* Updates the sickness status of each human as well as the grid accordingly */
void checkInfections(Grid grid, Human **humans, int *population, int gridColumns, int gridRows, Disease *disease);

/* Gets the error code */
char *getProgramError(ErrorCode e);

#endif
