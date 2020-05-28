#include "pipeline_utils.h"
#ifndef TEAM1FUNCTIONS
#define TEAM1FUNCTIONS

int execute_halt(Instruction intruction, struct CurrentState *state);

int execute_data_processing(Instruction intruction, struct CurrentState *state);

int execute_branch(Instruction intruction, struct CurrentState *state);


#endif
