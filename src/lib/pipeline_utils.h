// a header specifying all the structures and enums we use to represent the current state
// of our machines memory registers and the state of the pipeline
// as well as all the functions that alter these features

#include <stdint.h>
#ifndef PIPELINE_UTILS_H
#define PIPELINE_UTILS_H

#define MEMORY_SIZE 32768

typedef uint32_t Instruction;
typedef int32_t Register;

// structure for representing the current state of the memory and all registers
typedef struct CurrentState{
  // general purpose registers
  Register reg0;
  Register reg1;
  Register reg2;
  Register reg3;
  Register reg4;
  Register reg5;
  Register reg6;
  Register reg7;
  Register reg8;
  Register reg9;
  Register reg10;
  Register reg11;
  Register reg12;
  
  // SP register (ignored for this exercise)
  Register reg13;
  
  // LR register (ignored for this exercise)
  Register reg14;
  
  // PC register
  Register regPC;
  
  // CPSR register
  Register regCPSR;

  int8_t memory[MEMORY_SIZE];

  int branchFlag;
} State;

// structure for representing the currently
// fetched, decoded and executed instructions
typedef struct Pipeline{
  Instruction fetched;
  Instruction decoded;
  // Instruction executed;
} Pipeline;

// enum representing all 4 instruction types
// plus the all-0 - HALT instruction
typedef enum instr_type{
		BRANCH,
		DATA_TRANSFER,
		DATA_PROCESSING,
		MULTIPLY,
		HALT
} InstructionType;

// enum representing condition codes
// as hexadecimal numbers
typedef enum condition_code{
	       eq = 0x0,   // 0b0000
	       ne = 0x1,   // 0b0001
	       ge = 0xA,   // 0b1010
	       lt = 0xB,   // 0b1011
	       gt = 0xC,   // 0b1100
	       le = 0xD,   // 0b1101
	       al = 0xE    // 0b1110
} ConditionCode;

// at the termination of emulator
// prints out the values stored at the registers
// as well as any non-zero memory locations
void terminate(State *currentStatePtr);

//Returns an enum type specifying the type of the given instruction
InstructionType determineType(Instruction instruction);

// Determines the condition code of a given instruction
ConditionCode determineCondition(Instruction instruction);

// returns 1 if the condition code is satisfied
// by the current instruction, 0 otherwise
int determineValidity(Instruction instruction, State *statePtr);

// fetches an instruction from memory at the regPC address
// putting it into currentState.fetched
// shifts the pipeline and increments the PC
void fetchInstruction(State *currentStatePtr, Pipeline *currentPipelinePtr);

// sets or clears the C flag based on the value passed
void setC(State *statePtr, int value);

// sets the N flag to the 31st bit of the result
void setN(State *statePtr, int result);

// sets the Z flag iff the result is zero
void setZ(State *statePtr, int result);

#endif // PIPELINE_UTILS_H
