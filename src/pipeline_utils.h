// a header specifying all the structures and enums we use to represent the current state
// of our machines memory registers and the state of the pipeline
// as well as all the functions that alter these features

#include <stdint.h>
#ifndef PIPELINE_UTILS_H
#define PIPELINE_UTILS_H

#define MEMORY_SIZE 32768

typedef uint32_t Instruction;

// structure for representing the current state of the memory and all registers
typedef struct CurrentState{
  // general purpose registers
  uint32_t reg0;
  uint32_t reg1;
  uint32_t reg2;
  uint32_t reg3;
  uint32_t reg4;
  uint32_t reg5;
  uint32_t reg6;
  uint32_t reg7;
  uint32_t reg8;
  uint32_t reg9;
  uint32_t reg10;
  uint32_t reg11;
  uint32_t reg12;
  
  // SP register (ignored for this exercise)
  uint32_t reg13;
  
  // LR register (ignored for this exercise)
  uint32_t reg14;
  
  // PC register
  uint32_t regPC;
  
  // CPSR register
  uint32_t regCPSR;

  uint8_t memory[MEMORY_SIZE];
} State;

// structure for representing the currently
// fetched, decoded and executed instructions
typedef struct Pipeline{
  Instruction fetched;
  Instruction decoded;
  Instruction executed;
} Pipe;

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
// as hexadecinal numbers
typedef enum condition_code{
	       eq = 0x0,   // 0b0000
	       ne = 0x1,   // 0b0001
	       ge = 0xA,   // 0b1010
	       lt = 0xB,   // 0b1011
	       gt = 0xC,   // 0b1100
	       le = 0xD,   // 0b1101
	       al = 0xE    // 0b1110
} ConditionCode;

void terminate(struct CurrentState *currentState);

//Returns an enum type specifying the type of the given instruction
InstructionType determineType(Instruction instruction);

// Determines the condition code of a given instruction
ConditionCode determineCondition(Instruction instruction);

// returns 1 if the condition code is satisfied
// by the current instruction, 0 otherwise
int determineValidity(Instruction instruction, struct CurrentState *state);

// fetches an instruction from memory at the regPC address
// putting it into currentState.fetched
// shifts the pipeline and increments the PC
void fetchInstruction(struct CurrentState *currentState, struct Pipeline *currentPipeline);

#endif // PIPELINE_UTILS_H
