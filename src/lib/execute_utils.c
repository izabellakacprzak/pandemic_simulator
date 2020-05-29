#include "execute_utils.h"
#include <stdio.h>

#define CREATE_MASK(start, end) ((1 << start) - (1 << end))
//creates a mask of 1s from start to end

typedef int (*execution_function)(Instruction, State*);

static uint32_t and(uint32_t operand1, uint32_t operand2) {
  return 0;
}

static uint32_t eor(uint32_t operand1, uint32_t operand2) {
  return 0;
}

static uint32_t sub(uint32_t operand1, uint32_t operand2) {
  return 0;
}

static uint32_t rsb(uint32_t operand1, uint32_t operand2) {
  return 0;
}

static uint32_t add(uint32_t operand1, uint32_t operand2) {
  return 0;
}

static uint32_t mov(uint32_t operand1, uint32_t operand2) {
  return 0;
}

static uint32_t or(uint32_t operand1, uint32_t operand2) {
  return 0;
}

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
