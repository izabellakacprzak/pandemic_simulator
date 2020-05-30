#include <stdint.h>
#include <stdlib.h>
#include "execute_utils.h"
#include "pipeline_utils.h"

typedef int (*execution_function)(Instruction, State*);

int execute_halt(Instruction intruction, State *state){
  return 4;
}

int execute_data_processing(Instruction intruction, State *state) {
  return 2;
}


uint32_t *getRegPointer(int reg, State *currentState, Instruction instruction){
	uint32_t *regPtr = &currentState->reg0;
	// getting the 4 bits of the instruction
	// which correspond to the register which
	// starts at bit reg
	int regAddress = instruction & ((1 << (reg + 1)) - 1) >> (reg - 4);
	regPtr += regAddress;
	return regPtr;
}


int execute_multiply(Instruction instruction, State *state){
      //get destination register pointer - bits 19 to 16 in instr
      // Rm - bits 3 to 0
      // Rs - bits 11 to 8
      
      uint32_t *regRd = getRegPointer(19, state, instruction);
      uint32_t *regRs = getRegPointer(11, state, instruction);
      uint32_t *regRm = getRegPointer(3, state, instruction);

      
      // check whether to perform multiply and accumulate or
      // multiply only - function for now in pipeline_utils
      if(getA(instruction)){

	// set destination register to Rm x Rs + Rn
	// Rn - bits 15 to 12

	uint32_t *regRn = getRegPointer(15, state, instruction);

	*regRd = (*regRm) * (*regRs) + (*regRn);	

      } else{

	//set destination register to Rm x Rs
        *regRd = (*regRm) * (*regRs);
	
      }

      if(setCPSR(instruction)){

	//set result to the value in destination register
	int result = *regRd;
	setZ(state, result);
	setN(state, result);

      }

      // return 1 if successful;
      return 1;
}

int execute_data_transfer(Instruction instruction, State *state) {

  int offset = instruction & ((1 << 12) - 1);
	// here the function which calculates the offset
	// depending on the I bit of the instruction
  
	uint32_t *destReg = getRegPointer(15, state, instruction);
	uint32_t *baseReg = getRegPointer(19, state, instruction);
	
	if(!getU(instruction)){
		// subtracting offset
		offset = -offset;
	}
	
	if(getP(instruction)){
		// pre-indexing
		if(getL(instruction)){
			// loading
			*destReg = state->memory[*baseReg + offset];
		} else {
			// storing
			state->memory[*baseReg + offset] = *destReg;
		}
	} else {
		// post-indexing
		if(getL(instruction)){
			// loading
			*destReg = state->memory[*baseReg];
			*baseReg += offset;
		} else {
			// storing
			state->memory[*baseReg] = *destReg;
			*baseReg += offset;
		}
	}
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
