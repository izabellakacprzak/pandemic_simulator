#include <stdint.h>

#ifndef PIPELINE_UTILS_H
#define PIPELINE_UTILS_H

/* Registers stored as 32 bit unsigned ints:
     0-12  - general use
     13-14 - SP, LR (ignored for this)
     15    - PC
     16    - CPSR
 */

#define MEMORY_SIZE 32768

typedef uint32_t Instruction;

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

typedef struct Pipeline{
  Instruction fetched;
  Instruction decoded;
  Instruction executed;
} Pipe;

typedef enum instr_type{
		BRANCH,
		DATA_TRANSFER,
		DATA_PROCESSING,
		MULTIPLY,
		HALT
} InstructionType;

typedef enum condition_code{
	       eq = 0x0,   // 0b0000
	       ne = 0x1,   // 0b0001
	       ge = 0xA,   // 0b1010
	       lt = 0xB,   // 0b1011
	       gt = 0xC,   // 0b1100
	       le = 0xD,   // 0b1101
	       al = 0xE    // 0b1110
} ConditionCode;

#endif // PIPELINE_UTILS_H
