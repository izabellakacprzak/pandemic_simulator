#include "pipeline_utils.h"
#ifndef EXECUTE_UTILS
#define EXECUTE_UTILS

int execute_halt(Instruction intruction, State *state);

int execute_data_processing(Instruction intruction, State *state);

int execute_multiply(Instruction intruction, State *state);

int execute_data_transfer(Instruction intruction, State *state);

int execute_branch(Instruction intruction, State *state);

int execute(Instruction instruction, State *state, InstructionType type);


#endif
