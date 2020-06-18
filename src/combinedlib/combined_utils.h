#ifndef COMBINED_UTILS_H
#define COMBINED_UTILS_H

#include <stdint.h>

/* Typedef denoting instructions */
typedef uint32_t Instruction;

/* Typedef denoting label addresses */
typedef uint32_t Address;

/* Enum representing all 4 instruction types
   plus the all-0 - HALT instruction */
typedef enum instr_type{
  BRANCH,
  DATA_TRANSFER,
  DATA_PROCESSING,
  MULTIPLY,
  HALT,
  SHIFT
} InstructionType;

/* Enum representing the condition codes
   as hexadecimal numbers */
typedef enum condition_code{
  eq = 0x0,   // 0b0000
  ne = 0x1,   // 0b0001
  ge = 0xA,   // 0b1010
  lt = 0xB,   // 0b1011
  gt = 0xC,   // 0b1100
  le = 0xD,   // 0b1101
  al = 0xE    // 0b1110
} ConditionCode;

/* Enum representing the opcodes */
typedef enum opcode{
	AND = 0,
	EOR = 1,
	SUB = 2,
	RSB = 3,
	ADD = 4,
	TST = 8,
	TEQ = 9,
	CMP = 10,
	ORR = 12,
	MOV = 13,
	MUL = 15,
	MLA = 16,
	STR = 17,
	LDR = 18
} opcode;


/* Performs rotate right to the value by rotating cyclically 
   with bit 0 shifting into bit 31 */
int32_t ror(uint32_t value, uint32_t shift);

/* Performs rotate left to the value by rotating cyclically */
int32_t rol(uint32_t value, uint32_t shift);

#endif // COMBINED_UTILS_H
