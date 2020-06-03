/* 
   Note: memoryValue must be changed if Memory typedef is changed.
*/

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "pipeline_utils.h"

#define Z_MASK (1 << 30)
#define N_MASK (1 << 31)
#define C_MASK (1 << 29)

void terminate(State *statePtr){
  printf("Registers:\n");
  // prints the values stored a registers from 0 - 12
  for(int i = 0; i < 13; i++){
    printf("$%-3d: %10d (0x%08x)\n", i, statePtr->registers[i], statePtr->registers[i]);
  }

  // prints the values stored at registers PC and CPSR
  printf("PC  : %10d (0x%08x)\n", statePtr->regPC, statePtr->regPC);
  printf("CPSR: %10d (0x%08x)\n", statePtr->regCPSR, statePtr->regCPSR);

  // prints the values of non-zero memory locations
  printf("Non-zero memory:\n");
  uint32_t memoryValue = 0;
  for(int i = 0; i < MEMORY_SIZE; i+=4){
    // combining four 8-bit long ints into one 32-bit long
    memoryValue = (statePtr->memory[i] << 24);
    memoryValue += (statePtr->memory[i+1] << 16);
    memoryValue += (statePtr->memory[i+2] << 8);
    memoryValue += statePtr->memory[i+3];
    if(memoryValue != 0){
      printf("0x%08x: 0x%08x\n", i, memoryValue);
    }
    memoryValue = 0;
  }
}

// fetches an instruction from memory at the regPC address
// putting it into currentState.fetched
// shifts the pipeline and increments the PC
void fetchInstruction(State *statePtr, Pipeline *pipelinePtr){
  // Shifting the pipeline
  //currentPipelinePtr->executed = currentPipelinePtr->decoded;
  pipelinePtr->decoded = pipelinePtr->fetched;

  // Fetching next instruction and incrementing PC
  Memory first = statePtr->memory[statePtr->regPC];
  Memory second = statePtr->memory[statePtr->regPC + 1];
  Memory third = statePtr->memory[statePtr->regPC + 2];
  Memory fourth = statePtr->memory[statePtr->regPC + 3];

    
  pipelinePtr->fetched = first | (second << 8) | (third << 16) | (fourth << 24);
  statePtr->regPC += 4;
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

  if((1 << 7) & instruction && (1<<4) & instruction) {
    return MULTIPLY;
  }

  return DATA_PROCESSING;

}

// determines the condition code of a given instruction
static ConditionCode determineCondition(Instruction instruction){
  // takes first 4 bits of instruction
  return (instruction >> 28) & ((1 << 4) - 1);
}

// returns 1 if the condition code is satisfied
// by the current instruction, 0 otherwise
int determineValidity(Instruction instruction, State *statePtr){

  ConditionCode condition = determineCondition(instruction);
  int validity = 0;

  uint32_t setZ	= statePtr->regCPSR & Z_MASK;
  uint32_t clearZ = !(setZ);
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
// might be cleaner if we have this function and determine during the Data Processing execution
// whether we should set or clear C
void setC(State *statePtr, int value){

  if(value){
    statePtr->regCPSR = C_MASK | statePtr->regCPSR;
  } else{
    statePtr->regCPSR = ~C_MASK & statePtr->regCPSR;
  }
  
}
