#ifndef ASSEMBLE_UTILS_H
#define ASSEMBLE_UTILS_H

#include <stdint.h>
#include <errno.h>
#include "../combinedlib/combined_utils.h"
#include "tree.h"

#define MAX_INSTRUCTION_SIZE 511
#define MAX_EXPR_IN_BRACKETS 3
#define MAX_INSTRUCTION_PARAMS 5

/* Enum containing the error codes for error handling */
typedef enum errorCode {
  OK,
  INVALID_INSTRUCTION,
  INVALID_SHIFT,
  INVALID_INPUT,
  NOT_INSTRUCTION,
  WHITESPACE_LINE,
  OUT_OF_MEMORY,
  END_OF_FILE,
  NULL_FILE,
  SYS
} errorCode;

/* Struct containing the error message based on the error code*/
typedef struct err {
  int code;
  char *message;
} errorType;

#define EC_FROM_SYS_ERROR(e) (SYS + e)
#define EC_TO_SYS_ERROR(e) (e - SYS)
#define EC_IS_SYS_ERROR(e) (e >= SYS)

/* For program errors such as invalid instructions */
#define FATAL_PROG(pred, status) \
  do { if (pred) {currentStatus = status; goto fatalError;} } while (0)

/* For system errors such as failed file open */
#define FATAL_SYS(pred) \
  do { if (pred) {currentStatus = EC_FROM_SYS_ERROR(errno); goto fatalError;} } while (0)

/* Struct used for generating branch instructions */
typedef struct ldrAddresses {
  Address lastAddress;
  Address *currAddress;
  int length;
  Instruction *extraInstructions;
} ldrAddresses;

typedef enum shift_codes {
  LSL, 	// 0b00
  LSR,	// 0b01
  ASR,	// 0b10
  ROR	// 0b11     
} shift_c;

int assemble(Instruction *setInstruction, symbolNode *symbolTable,
	     char **nextInstruction, ldrAddresses *ldrAddresses);

int contains(char *value, const char **array);

char *getProgramError(errorCode e);

#endif // ASSEMBLE_UTILS_H

