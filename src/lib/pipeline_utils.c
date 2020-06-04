#include <stdint.h>
#include <stdio.h>
#include "pipeline_utils.h"

#define Z_MASK (1 << 30)
#define N_MASK (1 << 31)
#define C_MASK (1 << 29)

void terminate(State *statePtr) {
  /* Prints the values stored in the general purpose registers */
  printf("Registers:\n");
  for(int i = 0; i < 13; i++) {
    printf("$%-3d: %10d (0x%08x)\n", i, statePtr->registers[i], statePtr->registers[i]);
  }

  /* Prints the values stored in registers PC and CPSR */
  printf("PC  : %10d (0x%08x)\n", statePtr->regPC, statePtr->regPC);
  printf("CPSR: %10d (0x%08x)\n", statePtr->regCPSR, statePtr->regCPSR);

  /* Prints the contents in any non-zero memory location */
  printf("Non-zero memory:\n");
  uint32_t memoryValue = 0;
  for(int i = 0; i < MEMORY_SIZE; i+=4) {
    
    /* Combines four 8-bit long ints into one 32-bit long */
    memoryValue  = (statePtr->memory[i] << 24);
    memoryValue += (statePtr->memory[i+1] << 16);
    memoryValue += (statePtr->memory[i+2] << 8);
    memoryValue += statePtr->memory[i+3];
    
    if(memoryValue != 0) {
      printf("0x%08x: 0x%08x\n", i, memoryValue);
    }
    memoryValue = 0;
  }
}

void fetchInstruction(State *statePtr, Pipeline *pipelinePtr) {
  /* Shifting the pipeline */
  pipelinePtr->decoded = pipelinePtr->fetched;

  /* Fetching the next instruction and incrementing PC */
  Memory first = statePtr->memory[statePtr->regPC];
  Memory second = statePtr->memory[statePtr->regPC + 1];
  Memory third = statePtr->memory[statePtr->regPC + 2];
  Memory fourth = statePtr->memory[statePtr->regPC + 3];

    
  pipelinePtr->fetched = first | (second << 8) | (third << 16) | (fourth << 24);
  statePtr->regPC += 4;
}

/* This function supports only 4 of the 15 instructions 
   in the ARM instruction set; the rest are ignored for this exercise */
InstructionType determineType(Instruction instruction) {
  /* HALT - the all-0 instruction */
  if(instruction == 0x0) {
    return HALT;
  }

  /* BRANCH - 27th bit is set */
  if((1 << 27) & instruction) {
    return BRANCH;
  }

  /* DATA_TRANSFER - 26th bit is set */
  if((1 << 26) & instruction) {
    return DATA_TRANSFER;
  }

  /* DATA_PROCESSING with immediate constant - 25th bit is set */
  if((1 << 25) & instruction) {
    return DATA_PROCESSING;
  }

  /* MULTIPLY - the 4th and 7th bit are set */
  if((1 << 7) & instruction && (1<<4) & instruction) {
    return MULTIPLY;
  }

  /* Otherwise the instruction is DATA_PROCESSING */
  return DATA_PROCESSING;
}

/* Returns the first 4 bits in a given instruction - the condition code */
static ConditionCode determineCondition(Instruction instruction) {
  return (instruction >> 28) & ((1 << 4) - 1);
}


int determineValidity(Instruction instruction, State *statePtr) {
  ConditionCode condition = determineCondition(instruction);
  int validity = 0;

  /* Determines the state of the Z, N and V flags of the CPSR */
  uint32_t setZ	= statePtr->regCPSR & Z_MASK;
  uint32_t clearZ = !(setZ);
  uint32_t stateOfN = statePtr->regCPSR >> 31;
  uint32_t stateOfV = (statePtr->regCPSR << 3) >> 31;

  /* Checks whether the condition is fulfilled
     and updates validity accordingly */
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

void setZ(State *statePtr, int result) { 
  if(!result){
    statePtr->regCPSR = Z_MASK | statePtr->regCPSR;
  } else{
    statePtr->regCPSR = ~Z_MASK & statePtr->regCPSR;
  }
}


void setN(State *statePtr, int result) { 
  if(result & N_MASK){
    statePtr->regCPSR = N_MASK | statePtr->regCPSR;
  } else{
    statePtr->regCPSR = ~N_MASK & statePtr->regCPSR;    
  }
}

void setC(State *statePtr, int value) {
  if(value){
    statePtr->regCPSR = C_MASK | statePtr->regCPSR;
  } else{
    statePtr->regCPSR = ~C_MASK & statePtr->regCPSR;
  }
}
