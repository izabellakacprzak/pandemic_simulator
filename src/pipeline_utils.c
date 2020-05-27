#include <stdint.h>
#include <stdlib.h>
#include "pipeline_utils.h"
//Define masks for the particular bits instead of creating them in the if?

#define Z_MASK (1 << 30)

//Returns an enum type specifying the type of the given instruction

InstructionType determineType(Instruction instruction){

  //HALT = all-0 instruction
  //Give the value directly to the enum?
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

  //DATA_PROCESSING with imm constant - 25th bit is set
  //DATA_PROCESSING with shifted register specified by const - 4th bit is clear
  //DATA_PROCESSING with shifted register specified by register - 7th bit is clear
  if((1 << 25) & instruction){
    return DATA_PROCESSING;
  }

  if(~(1 << 7) & instruction){
    return DATA_PROCESSING;
  }

  if(~(1 << 4) & instruction){
    return DATA_PROCESSING;
  }

  return MULTIPLY;

}

// Determines the condition code of a given instruction
// Should we return enum value or just an int?

ConditionCode determineCondition(Instruction instruction){
  // Takes first 4 bits of instruction
  return instruction & ~((1 << 29) - 1);
}


//Won't compile because CurrentState is declared in emulator
/*Should we consider putting our main structs in a header file for readability and easier access? */ 

int determineValidity(Instruction instruction, struct CurrentState state){

  ConditionCode condition = determineCondition(instruction);
  int validity = 0;

  uint32_t setZ = state.regCPSR & Z_MASK;
  uint32_t clearZ = !(setZ);

  uint32_t stateOfN = state.regCPSR >> 31;
  uint32_t stateOfV = (state.regCPSR << 3) >> 31;

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

