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
Register *getRegPointer(int reg, State *currentState, Instruction instruction);

int execute_halt(Instruction instruction, State *state);

Register get_offset_register(int carry, Instruction instruction, State *state);

int execute_data_processing(Instruction instruction, State *state);

int execute_multiply(Instruction instruction, State *state);

int execute_data_transfer(Instruction instruction, State *state);

int execute_branch(Instruction instruction, State *state);

int execute(Instruction instruction, State *state, InstructionType type);


#endif
