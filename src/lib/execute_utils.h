#ifndef EXECUTE_UTILS
#define EXECUTE_UTILS

#include "pipeline_utils.h"

/* Struct for calling the functions in the DATA_PROCESSING instruction - 
   determines the type of the operator, whether the result is written 
   to the destination register and has a function pointer to the operation function */
typedef struct Operator {
  int isArithmetic;
  int isWritten;
  union {
    Register (*operationWithoutCarry) (Register, Register);
    Register (*operationWithCarry) (int, State *, Register, Register);
  } operation;
} Operator;

/* Struct for the different flags which appear in 
   the instruction set of the emulator with their value 
   set to their respective position in an instruction */
typedef enum Flags {
  S = 20,
  L = 20,
  A = 21,
  U = 23,
  P = 24,
  I = 25
} Flags;

/* Returns a pointer to a register 
   whose address starts at bit reg and is 4 bits long */
Register *getRegPointer(int reg, State *statePtr, Instruction instruction);

/* Executes a decoded instruction of InstructionType type */
int execute(Instruction instruction, State *statePtr, InstructionType type);

#endif
