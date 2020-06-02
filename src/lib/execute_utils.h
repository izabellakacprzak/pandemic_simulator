#include "pipeline_utils.h"
#ifndef EXECUTE_UTILS
#define EXECUTE_UTILS

typedef struct Operator {
  int isArithmetic;
  int isWritten;
  union {
    Register (*operationWithoutCarry) (Register, Register);
    Register (*operationWithCarry) (int, State *, Register, Register);
  } operation;
} Operator;

typedef enum flags{
  S = 20,
  L = 20,
  A = 21,
  U = 23,
  P = 24,
  I = 25
} Flags;


// return a pointer to a register whose address
// is the 4 bits of instruction starting from bit reg
Register *getRegPointer(int reg, State *statePtr, Instruction instruction);

int execute(Instruction instruction, State *statePtr, InstructionType type);

#endif
