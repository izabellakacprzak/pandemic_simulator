#ifndef ASSEMBLE_UTILS_H
#define ASSEMBLE_UTILS_H

#include <stdint.h>
#include <errno.h>

#include "../combinedlib/combined_utils.h"
#include "tree.h"

#define MAX_INSTRUCTION_SIZE 511
#define MAX_EXPR_IN_BRACKETS 3

// this is needed for tree.h
// typedef uint32_t Address;

typedef enum errorCode {
  OK, 
  INVALID_INSTRUCTION, 
  SYS
} errorCode;

typedef struct err {
  int code;
  char *message;
} errorType;

#define EC_FROM_SYS_ERROR(e) (SYS + e)
#define EC_TO_SYS_ERROR(e) (e - SYS)
#define EC_IS_SYS_ERROR(e) (e >= SYS)

//for program errors such as invalid instructions
#define FATAL_PROG(pred, status) \
  do { if (pred) {currentStatus = status; goto fatalError;} } while (0)

//for system errors such as failed file open
#define FATAL_SYS(pred) \
  do { if (pred) {currentStatus = EC_FROM_SYS_ERROR(errno); goto fatalError;} } while (0)

typedef struct ldrAddresses {
  Address lastAddress;
  int length;
  Instruction *extraInstructions;
} ldrAddresses;

typedef enum shift_codes {
  lsl_c, 	// 0b00
  lsr_c,	// 0b01
  asr_c,	// 0b10
  ror_c		// 0b11     
} shift_c;

Instruction assemble(symbolNode *symbolTable, const char *nextInstruction);

int contains(char *value, const char **array);

char *getProgramError(errorCode e);

#endif // ASSEMBLE_UTILS_H

