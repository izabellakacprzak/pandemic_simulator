#include "pipeline_utils.h"
#ifndef EXECUTE_UTILS
#define EXECUTE_UTILS

typedef struct Operand {
  Register (*operation) (Register, Register);
  int isArithmetic;
  int isWritten;
} Operand;

// return a pointer to a register whose address
// is the 4 bits of instruction starting from bit reg
Register *getRegPointer(int reg, State *currentState, Instruction instruction);

int execute_halt(Instruction intruction, State *state);

int execute_data_processing(Instruction intruction, State *state);

int execute_multiply(Instruction intruction, State *state);

int execute_data_transfer(Instruction intruction, State *state);

int execute_branch(Instruction intruction, State *state);

int execute(Instruction instruction, State *state, InstructionType type);


#endif
