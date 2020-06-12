#ifndef ASSEMBLE_UTILS_H
#define ASSEMBLE_UTILS_H

#include <stdint.h>
#include <errno.h>

#include "../combinedlib/combined_utils.h"
#include "tree.h"

#define MAX_INSTRUCTION_SIZE 511
#define MAX_EXPR_IN_BRACKETS 3

typedef enum errorCode {
  OK, 
  INVALID_INSTRUCTION,
  INVALID_SHIFT,
  INVALID_INPUT,
  END_OF_FILE,
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
  LSL, 	// 0b00
  LSR,	// 0b01
  ASR,	// 0b10
  ROR	// 0b11     
} shift_c;

typedef enum data_processing_codes{
	eor = 1,
	sub = 2,
	rsb = 3,
	add = 4,
	orr = 12,
	mov = 13,
	tst = 8,
	teq = 9,
	cmp = 10
} data_processing_c;

Instruction assemble(symbolNode *symbolTable, char **nextInstruction);

int contains(char *value, const char **array);

char *getProgramError(errorCode e);

#endif // ASSEMBLE_UTILS_H

