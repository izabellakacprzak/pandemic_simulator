#include "execute_utils.h"
#include "pipeline_utils.h"

typedef int (*execution_function)(Instruction, State*);

int execute_halt(Instruction intruction, State *state){
  return 4;
}

int execute_data_processing(Instruction intruction, State *state) {
  return 2;
}

int execute_multiply(Instruction intruction, State *state){
  if(determineValidity(instruction, *state){

      //get destination register pointer - bits 19 to 16 in instr

      uint32_t *destPtr;
      
      // check whether to perform multiply and accumulate or
      // multiply only - function for now in pipeline_utils
      if(getA(instruction)){

	// set destination register to Rm x Rs + Rn
	// Rn - bits 15 to 12
	// Rm - bits 3 to 0
	// Rs - bits 11 to 8

      } else{

	//set destination register to Rm x Rs
	// Rm - bits 3 to 0
	// Rs - bits 11 to 8
	
      }

      if(setCPSR(instruction)){

	//set result to the value in destination register
	int result;
	setZ(state, result);
	setN(state, result);

      }

      // return 1 if successful;
    }

    //return 0 if unsuccessful;
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
