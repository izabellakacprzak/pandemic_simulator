#include "execute_utils.h"

int execute(Instruction instruction, struct CurrentState *state, enum instr_type type) {
  //instruction to be executed, machine state, boolean on whether to execute, instruction type
  int (*executions[5])(Instruction, struct CurrentState*);

  //initialises the array with function pointers for each instruction type
  //based on the ordering of instr_type enum
  executions[0] = execute_branch;
  executions[1] = execute_data_transfer;
  executions[2] = execute_data_processing;
  executions[3] = execute_multiply;
  executions[4] = execute_halt;
  
  return (executions[type](instruction, state));
  //executes the function corresponding to the type of instruction by the enum's value
}
