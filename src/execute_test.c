#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "pipeline_utils.h"


int execute_halt(Instruction intruction, struct CurrentState *state){
  return 4;
}

int execute_data_processing(Instruction intruction, struct CurrentState *state) {
  return 2;
}

int execute_multiply(Instruction intruction, struct CurrentState *state){
  return 3;
}

int execute_data_transfer(Instruction intruction, struct CurrentState *state) {
  return 1;
}

int execute_branch(Instruction intruction, struct CurrentState *state){
  return 0;
}

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

int main(void) {
  //TEST METHOD FOR EXECUTE LOADER
  enum instr_type current_type[5]= {BRANCH, DATA_TRANSFER, DATA_PROCESSING, MULTIPLY, HALT};
  struct CurrentState state = {0};
  Instruction instruction = 0;
  int success = 1;
  int type_select;
  
  for(int i = 0; i < 50000; i++) {
    type_select = rand() % 5;
    if (execute(instruction,&state,current_type[type_select]) != type_select) {
      printf("TEST FAILED AT INDEX %d\n", i);
      success = 0;
    } 
  }

  if (success) {
    printf("ALL TESTS OK\n");
  }

  return 0;
}
