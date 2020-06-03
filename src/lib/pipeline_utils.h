#ifndef PIPELINE_UTILS_H
#define PIPELINE_UTILS_H

#include <stdint.h>

#define MEMORY_SIZE (1 << 15)
#define REGISTERS_SIZE (17)
#define regPC registers[15]
#define regCPSR registers[16]

typedef uint32_t Instruction;
typedef int32_t Register;
typedef uint8_t Memory;

/* Struct for the current state of the memory
   and the registers of the emulator */
typedef struct State {
  
  /* registers[0-12] -> general purpose
     registers[13]   -> SP register (ignored for this exercise)
     registers[14]   -> LR register (ignored for this exercise)
     registers[15]   -> PC register
     registers[16]   -> CPSR register */
  
  Register registers[REGISTERS_SIZE];
  Memory memory[MEMORY_SIZE];

  /* Branch flag, set if a branch instruction has been exectued;
     clear by default */
  int branchFlag;
  
} State;

/* Struct for the simulation of an ARM pipeline */
typedef struct Pipeline {
  
  Instruction fetched;
  Instruction decoded;
  
} Pipeline;

/*  Enum for the 4 types of instructions
    plus the all-0 - HALT - instruction */
typedef enum InstructionType {
			      
  BRANCH,
  DATA_TRANSFER,
  DATA_PROCESSING,
  MULTIPLY,
  HALT
  
} InstructionType;

/* Enum for the condition suffixes with their 
   respective codes represented as hexadecimal numbers */
typedef enum ConditionCode {
			    
  eq = 0x0,   // 0b0000
  ne = 0x1,   // 0b0001
  ge = 0xA,   // 0b1010
  lt = 0xB,   // 0b1011
  gt = 0xC,   // 0b1100
  le = 0xD,   // 0b1101
  al = 0xE    // 0b1110
  
} ConditionCode;

/* Upon termination prints out the state of the registers 
   and the contents of any non-zero memory location */
void terminate(State *statePtr);

/* Specifies the type of a given instruction 
   by returning the corresponding enum */
InstructionType determineType(Instruction instruction);

/* Returns 1 if the condition of the instruction succeeds,
   0 otherwise */
int determineValidity(Instruction instruction, State *statePtr);

/* Fetches an instruction from memory and puts it 
   into currentState.fetched; shifts the pipeline and increments the PC */
void fetchInstruction(State *statePtr, Pipeline *pipelinePtr);

/* Clears the C flag if the value passed is 0,
   sets it to 1 otherwise */
void setC(State *statePtr, int value);

/* Sets the N flag to the 31st bit of the result */
void setN(State *statePtr, int result);

/* Sets the Z flag if the result is 0,
   clears it otherwise */
void setZ(State *statePtr, int result);

#endif // PIPELINE_UTILS_H
