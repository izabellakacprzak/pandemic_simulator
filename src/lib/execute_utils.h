#include "pipeline_utils.h"
#ifndef EXECUTE_UTILS
#define EXECUTE_UTILS

int execute_halt(Instruction intruction, struct CurrentState *state);

int execute_data_processing(Instruction intruction, struct CurrentState *state);

int execute_multiply(Instruction intruction, struct CurrentState *state);

int execute_data_transfer(Instruction intruction, struct CurrentState *state);

int execute_branch(Instruction intruction, struct CurrentState *state);

int execute(Instruction instruction, struct CurrentState *state, enum instr_type type);


#endif
