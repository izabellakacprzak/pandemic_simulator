#include <stdint.h>

#ifndef PIPELINE_UTILS_H
#define PIPELINE_UTILS_H

typedef uint32_t Instruction;

typedef enum instr_type{
		BRANCH,
		DATA_TRANSFER,
		DATA_PROCESSING,
		MULTIPLY,
		HALT
} InstructionType;

typedef enum condition_code{
	       eq = 0b0000,
	       ne = 0b0001,
	       ge = 0b1010,
	       lt = 0b1011,
	       gt = 0b1100,
	       le = 0b1101,
	       al = 0b1110
} ConditionCode;

#endif //PIPELINE_UTILS_H
