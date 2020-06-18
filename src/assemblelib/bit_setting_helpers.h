#ifndef ASSEMBLE_HELPERS
#define ASSEMBLE_HELPERS

#include "assemble_utils.h"
#include "tree.h"

/* Set bits starting from "bit" to a given value */
Instruction setBits(uint32_t value, int bit, Instruction instr);

/* Calculates the second operand in Data Transfer and Data Processing instructions */
int calculateOperand(Instruction *instruction, char **code, int opIndex, int processing);

/* Set bits for data processing instructions that compute results: 
   and, eor, sub, rsb, add, orr */
int withResults(Instruction *instruction, char **code);

/* Set bits for data processing instructions that 
   perform single operand assignment: mov */
int operandAssignment(Instruction *instruction, char **code);

/* Set bits for instructions that do not compute results 
   but set the CPSR flags: tst, teq, cmp */
int noResults(Instruction *instruction, char **code);

/* Sets register related bits for multiply instructions*/
int setRegistersMultiply(Instruction *instruction, char **code);

/* Take an expression with or without [] brackets, return the number of arguments in brackets */
int removeBrackets(char **destTok, char *expression);

/* sets bits for ldr Rd,=<expr> type dataTransfer instructions */
int dataTransferImmediate(ldrAddresses *ldrAddresses, char **code, Instruction *instruction);

#endif
