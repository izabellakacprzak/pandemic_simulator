#include "execute_utils.h"
#include <stdio.h>

#define CREATE_MASK(start, end) ((1 << (start + 1)) - (1 << end))
//creates a mask of 1s from start to end
#define OPERATOR_FUNCTION(name, operator)	\
  static uint32_t name(const uint32_t operand1, const uint32_t operand2)\
  {\
    return operand1 operator operand2;\
  }

typedef int (*execution_function)(Instruction, State*);

// defining operator functions for use in execute_data_processing
OPERATOR_FUNCTION(and, &)
OPERATOR_FUNCTION(eor, ^)
OPERATOR_FUNCTION(sub, -)
OPERATOR_FUNCTION(add, +)
OPERATOR_FUNCTION(or, |)
OPERATOR_FUNCTION(mov, *0+)

static uint32_t rsb(uint32_t operand1, uint32_t operand2) {
  return sub(operand2, operand1);
}

OPERATOR_FUNCTION(lsl, <<)
OPERATOR_FUNCTION(lsr, >>)

static uint32_t asr(uint32_t value, uint32_t shift){
  if (shift > 32) {
    shift = 32;
  }

  int sign = value & (1 << 31);
  value = value >> shift;

  if (sign) {
    return value | ~((1 << (32 - shift)) - 1); // CREATE_MASK(31, (32 - shift));
  }

  return value;
}

static uint32_t ror(uint32_t value, uint32_t shift){
  uint32_t shifted, rotated;
  shift %= 32;

  shifted = value >> shift;
  rotated = value << (32 - shift);
  return shifted | rotated;
}

// start of execute functions
int execute_halt(Instruction intruction, State *state){
  return 4;
}

static int execute_data_processing(Instruction instruction, State *state) {
  uint32_t operand1, operand2, destination;
  Operand operand = {0};
  
  //operand1 = *(state + (instruction & CREATE_MASK(19,16)));
  //destination = *(state + (instruction & CREATE_MASK(15,12)));
  //TODO fix these lines
  switch(instruction & CREATE_MASK(24,21)) {
    case 0: 
      operand.operation = and;
      operand.isWritten = 1;
      break;
    case 1:
      operand.operation = eor;
      operand.isWritten = 1;
      break;
    case 2:
      operand.operation = sub;
      operand.isWritten = 1;
      operand.isArithmetic = 1;
      break;
    case 3:
      operand.operation = rsb;
      operand.isWritten = 1;
      operand.isArithmetic = 1;
      break;
    case 4:
      operand.operation = add;
      operand.isWritten = 1;
      operand.isArithmetic = 1;
      break;
    case 8:
      operand.operation = and;
      break;
    case 9:
      operand.operation = eor;
      break;
    case 10:  
      operand.operation = sub;
      operand.isArithmetic = 1;
      break;
    case 12:
      operand.operation = or;
      operand.isWritten = 1;
      break;
    case 13:
      operand.operation = mov;
      operand.isWritten = 1;
      break;
    default:
      perror("ERROR: invalid operand");
      return 1;
  }
  
  if (instruction & (1 << 25)) {
    // operand2 is an immediate constant
    operand2 = instruction & CREATE_MASK(7, 0);
    // rotate  = rotate * 2
    uint32_t rotate = (instruction & CREATE_MASK(11, 8)) >> 7;

    operand2 = ror(operand2, rotate);
  }
  
  return 0;
}

int execute_multiply(Instruction intruction, State *state){
  return 3;
}

int execute_data_transfer(Instruction intruction, State *state) {
  return 1;
}

int execute_branch(Instruction instruction, State *state){
  return 0;
}

int execute(Instruction instruction, State *state, InstructionType type) {
  //instruction to be executed, machine state, boolean on whether to execute, instruction type
  execution_function executions[5];

  //initialises the array with function pointers for each instruction type
  //based on the ordering of instr_type enum
  executions[0] = execute_branch;
  executions[1] = execute_data_transfer;
  executions[2] = execute_data_processing;
  executions[3] = execute_multiply;
  executions[4] = execute_halt;
  
  return (executions[type](instruction, state));
  //executes the function corresponding to the type of instruction by the enum's value
}
