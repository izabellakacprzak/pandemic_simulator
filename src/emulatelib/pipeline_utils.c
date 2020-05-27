#include <stdint.h>
#include <stdlib.h>
#include "pipeline_utils.h"

#define Z_MASK (1 << 30)

// fetches an instruction from memory at the regPC address
// putting it into currentState.fetched
// shifts the pipeline and increments the PC
void fetchInstruction(struct CurrentState *currentStatePtr, struct Pipeline *currentPipelinePtr){
  // Shifting the pipeline
  currentPipelinePtr->executed = currentPipelinePtr->decoded;
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
int determineValidity(Instruction instruction, struct CurrentState *statePtr){

  ConditionCode condition = determineCondition(instruction);
  int validity = 0;

  uint32_t setZ	= statePtr->regCPSR & Z_MASK;
  uint32_t clearZ = ~(setZ);
  uint32_t stateOfN = statePtr->regCPSR >> 31;
  uint32_t stateOfV = (statePtr->regCPSR << 3) >> 31;

  // checks for all possible conditions and updates validity accordingly
  switch(condition) {
  case eq:validity = setZ;
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