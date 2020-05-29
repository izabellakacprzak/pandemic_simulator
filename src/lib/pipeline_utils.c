#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "pipeline_utils.h"

#define Z_MASK (1 << 30)
#define N_MASK (1 << 31)
#define C_MASK (1 << 29)

void terminate(State *currentState){
	printf("Registers:\n");
        uint32_t *regPtr = &currentState->reg0;
	// prints the values stored a registers from 0 - 12
        for(int i = 0; i < 13; i++){
                printf("$%-3d:%11u (0x%08x)\n", i, *regPtr, *regPtr);
                regPtr++;
        }

	// ptints the values stored at registers PC and CPSR
	printf("PC  :%11u (0x%08x)\n", currentState->regPC, currentState->regPC);
	printf("CPSR:%11u (0x%08x)\n", currentState->regCPSR, currentState->regCPSR);

	// prints the values of non-zero memory locations
        printf("Non-zero memory:\n");
	uint32_t memoryValue = 0;
        for(int i = 0; i < MEMORY_SIZE; i+=4){
		// combinitng four 8-bit long ints into one 32-bit long
		memoryValue = (currentState->memory[i] << 24);
		memoryValue += (currentState->memory[i+1] << 16);
		memoryValue += (currentState->memory[i+2] << 8);
		memoryValue += currentState->memory[i+3];
                if(memoryValue != 0){
                        printf("0x%08x: 0x%08x\n", i, memoryValue);
                }
		memoryValue = 0;
        }
}

// fetches an instruction from memory at the regPC address
// putting it into currentState.fetched
// shifts the pipeline and increments the PC
void fetchInstruction(State *currentStatePtr, Pipeline *currentPipelinePtr){
  // Shifting the pipeline
  //currentPipelinePtr->executed = currentPipelinePtr->decoded;
  currentPipelinePtr->decoded = currentPipelinePtr->fetched;

  // Fetching next instruction and incramenting PC
  uint8_t first = currentStatePtr->memory[currentStatePtr->regPC];
  uint8_t second = currentStatePtr->memory[currentStatePtr->regPC + 1];
  uint8_t third = currentStatePtr->memory[currentStatePtr->regPC + 2];
  uint8_t fourth = currentStatePtr->memory[currentStatePtr->regPC + 3];

    
  currentPipelinePtr->fetched = (first << 24) | (second << 16) | (third << 8) | fourth;
  currentStatePtr->regPC += 4;
}

//Returns an enum type specifying the type of the given instruction
InstructionType determineType(Instruction instruction){
  //HALT = all-0 instruction all bits are 0
  if(instruction == 0x0){
    return HALT;
  }

  //BRANCH - 27th bit is set
  if((1 << 27) & instruction){
    return BRANCH;
  }

  //DATA_TRANSFER - 26th bit is set
  if((1 << 26) & instruction){
    return DATA_TRANSFER;
  }

  //DATA_PROCESSING with immediate constant - 25th bit is set
   if((1 << 25) & instruction){
    return DATA_PROCESSING;
  }

  //DATA_PROCESSING with shifted register specified by const - 7th bit is clear
  if(~(1 << 7) & instruction){
    return DATA_PROCESSING;
  }
  
  //DATA_PROCESSING with shifted register specified by register - 4th bit is clear 
  if(~(1 << 4) & instruction){
    return DATA_PROCESSING;
  }

  return MULTIPLY;

}

// determines the condition code of a given instruction
ConditionCode determineCondition(Instruction instruction){
  // takes first 4 bits of instruction
  return instruction & ~((1 << 29) - 1);
}

// returns 1 if the condition code is satisfied
// by the current instruction, 0 otherwise
int determineValidity(Instruction instruction, State *statePtr){

  ConditionCode condition = determineCondition(instruction);
  int validity = 0;

  uint32_t setZ	= statePtr->regCPSR & Z_MASK;
  uint32_t clearZ = ~(setZ);
  uint32_t stateOfN = statePtr->regCPSR >> 31;
  uint32_t stateOfV = (statePtr->regCPSR << 3) >> 31;

  // checks for all possible conditions and updates validity accordingly
  switch(condition) {
  case eq: validity = setZ;
    break;
  case ne: validity = clearZ;
    break;
  case ge: validity = (stateOfN == stateOfV);
    break; 
  case lt: validity = (stateOfN != stateOfV);
    break;
  case gt: validity = clearZ && (stateOfN == stateOfV);
    break;
  case le: validity = setZ || (stateOfN != stateOfV);
    break;
  case al: validity = 1;
    break;
  default: validity = 0;
  }

  return validity;

}

// determines whether the CPSR flags should be updated
// takes the 20th bit of an instruction
uint32_t setCPSR(Instruction instruction){

  return instruction & (1 << 20);
  
}



// sets the Z flag iff the result is zero
void setZ(State *statePtr, int result){

  if(!result){
    statePtr->regCPSR = Z_MASK | statePtr->regCPSR;
  } else{
    statePtr->regCPSR = ~Z_MASK & statePtr->regCPSR;
  }

}

// sets the N flag to the 31st bit of the result
void setN(State *statePtr, int result){

  if(result & N_MASK){
    statePtr->regCPSR = N_MASK | statePtr->regCPSR;
  } else{
    statePtr->regCPSR = ~N_MASK & statePtr->regCPSR;    
  }
  
}

// sets or clears the C flag based on the value passed
// there are too many conditions which determine whether C should be set or cleared
// might be cleaner if we have this funvtion and determine during the Data Processing execution
// whether we should set or clear C
void setC(State *statePtr, int value){

  if(value){
    statePtr->regCPSR = C_MASK | statePtr->regCPSR;
  } else{
    statePtr->regCPSR = ~C_MASK & statePtr->regCPSR;
  }
  
}



// Should we have different names opposed to getX?



// determines whether the multiply instrucion should perform
// multiply and accumulate or multiply only
// takes the 21st bit of an instruction
uint32_t getA(Instruction instruction){

  return instruction & (1 << 21);
  
}



// determines whether Offset is interpreted as a shifted register
// or as an unsigned 12 bit imm offset
// takes the 25th bit of an instrucion
uint32_t getI(Instruction instruction){

  return instruction & (1 << 25);

}

// determines  whether the offset is added/subtracted to the base register
// before transferring the data or after
// takes the 24th bit of an instruction
uint32_t getP(Instruction instruction){

  return instruction & (1 << 24);
  
}


// determines whether the offset is added to or
// subtracted from the base register
// takes the 23th bit of instruction
uint32_t getU(Instruction instruction){

  return instruction & (1 << 23);
  
}


//determines whether the word is loaded from memory
// or stored into
//takes the 20th bit of an instruction
//currently the same instruction as setCPSR - should we have both for clarity sake?
uint32_t getL(Instruction instruction){

  return instruction & (1 << 20);

}

