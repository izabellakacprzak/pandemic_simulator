#include "execute_utils.h"

typedef int (*execution_function)(Instruction, State*);

int execute_halt(Instruction intruction, State *state){
  return 4;
}

int execute_data_processing(Instruction intruction, State *state) {
  return 2;
}

int execute_multiply(Instruction intruction, State *state){
  return 3;
}

int execute_data_transfer(Instruction intruction, State *state) {
  return 1;
}

int execute_branch(Instruction intruction, State *state){
  return 0;
}

int execute(Instruction instruction, State *state, InstructionType type) {
  //instruction to be executed, machine state, boolean on whether to execute, instruction type
  execution_function executions[5];

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
