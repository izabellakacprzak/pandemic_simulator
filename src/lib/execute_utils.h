#include "pipeline_utils.h"
#ifndef EXECUTE_UTILS
#define EXECUTE_UTILS

typedef struct Operand {
  uint32_t (*operation) (uint32_t,uint32_t);
  int isArithmetic;
  int isWritten;
} Operand;

int execute_halt(Instruction intruction, State *state);

static int execute_data_processing(Instruction intruction, State *state);

int execute_multiply(Instruction intruction, State *state);

int execute_data_transfer(Instruction intruction, State *state);

int execute_branch(Instruction intruction, State *state);

int execute(Instruction instruction, State *state, InstructionType type);


#endif
