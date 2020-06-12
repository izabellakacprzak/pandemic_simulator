#ifndef COMBINED_UTILS_H
#define COMBINED_UTILS_H

#include <stdint.h>

typedef uint32_t Instruction;

// The type for label addresses
typedef uint32_t Address;

// enum representing all 4 instruction types
// plus the all-0 - HALT instruction
typedef enum instr_type{
  BRANCH,
  DATA_TRANSFER,
  DATA_PROCESSING,
  MULTIPLY,
  HALT,
  SHIFT
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


typedef enum opcode{
	AND = 0,
	EOR = 1,
	SUB = 2,
	RSB = 3,
	ADD = 4,
	ORR = 12,
	MOV = 13,
	TST = 8,
	TEQ = 9,
	CMP = 10
} opcode;


/* Performs rotate right to the value by rotating cyclically 
   with bit 0 shifting into bit 31 */
int32_t ror(uint32_t value, uint32_t shift);
int32_t rol(uint32_t value, uint32_t shift);

#endif // COMBINED_UTILS_H
