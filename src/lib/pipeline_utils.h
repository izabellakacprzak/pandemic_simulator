// a header specifying all the structures and enums we use to represent the current state
// of our machines memory registers and the state of the pipeline
// as well as all the functions that alter these features

#include <stdint.h>
#ifndef PIPELINE_UTILS_H
#define PIPELINE_UTILS_H

#define MEMORY_SIZE 32768

typedef uint32_t Instruction;
typedef int32_t Register;

typedef union RegisterList {
  struct {
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
  } regStruct;
  Register regArray[17];
} RegisterList;

// structure for representing the current state of the memory and all registers
typedef struct CurrentState{
  RegisterList registers;
  int8_t memory[MEMORY_SIZE];
  int32_t branchFlag;
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

// determines whether the CPSR flags should be updated
// takes the 20th bit of an instruction
int setCPSR(Instruction instruction);

// determines whether the multiply instrucion should perform
// multiply and accumulate or multiply only
// takes the 21st bit of an instruction
int getA(Instruction instruction);

// determines whether Offset is interpreted as a shifted register
// or as an unsigned 12 bit imm offset
// takes the 25th bit of an instrucion
int getI(Instruction instruction);

// determines  whether the offset is added/subtracted to the base register
// before transferring the data or after
// takes the 24th bit of an instruction
int getP(Instruction instruction);

// determines whether the offset is added to or
// subtracted from the base register
// takes the 23th bit of instruction
int getU(Instruction instruction);

//determines whether the word is loaded from memory
// or stored into
//takes the 20th bit of an instruction
int getL(Instruction instruction);

#endif // PIPELINE_UTILS_H
