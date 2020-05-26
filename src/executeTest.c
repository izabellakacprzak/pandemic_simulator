#include <stdint.h>
#define MEMORY_SIZE 2
enum InstructType {HALT = 0, DATA_PROCESSING = 1, MULTIPLY = 2, SINGLE_DATA_TRANSFER = 3, BRANCH = 4};

struct CurrentState{
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
};

int execute_halt( int intruction, struct CurrentState state, int perform);

int execute_data_processing( int intruction, struct CurrentState state, int perform);

int execute_multiply( int intruction, struct CurrentState state, int perform);

int execute_data_transfer( int intruction, struct CurrentState state, int perform);

int execute_branch( int intruction, struct CurrentState state, int perform);

int execute(int instruction, struct CurrentState state, int perform,enum InstructType type) {
  int (*executions[5])(int, struct CurrentState, int);
  executions[0] = execute_halt;
  executions[1] = execute_data_processing;
  executions[2] = execute_multiply;
  executions[3] = execute_data_transfer;
  executions[4] = execute_branch;
  return executions[type](instruction, state, perform);
}
