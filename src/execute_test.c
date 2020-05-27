#include <stdint.h>
#include "pipeline_utils.h"


int execute_halt( int intruction, struct CurrentState state, int perform);

int execute_data_processing( int intruction, struct CurrentState state, int perform);

int execute_multiply( int intruction, struct CurrentState state, int perform);

int execute_data_transfer( int intruction, struct CurrentState state, int perform);

int execute_branch( int intruction, struct CurrentState state, int perform);

int execute(int instruction, struct CurrentState state, int perform, enum InstructType type) {
  int (*executions[5])(int, struct CurrentState, int);

  
  executions[0] = execute_halt;
  executions[1] = execute_data_processing;
  executions[2] = execute_multiply;
  executions[3] = execute_data_transfer;
  executions[4] = execute_branch;
  

  
  return executions[type](instruction, state, perform);
}
