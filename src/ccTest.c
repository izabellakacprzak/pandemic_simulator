#include <stdint.h>
#include <stdio.h>
#define MEMORY_SIZE 2


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

int cpsrCheck(struct CurrentState state, int mask, int *expected, int no_expected) {
  int res;
  mask = mask << 28;
  res = state.regCPSR & mask;
  for (int i = 0; i < no_expected; i++) {
    if (res == expected[i]) {
      return 1;
    }
  }
  return 0;
}

int checkCC(int instruction,struct CurrentState state) {
  int mask = (2 << 32) - (2 << 28);
  int masked = instruction & mask;
  int cmp[3];
  
  switch (masked) {
  case 0:
    cmp[0] = 1;
    return cpsrCheck(state, 4, cmp, 1);
  case 1:
    cmp[0] = 0;
    return cpsrCheck(state, 4, cmp, 1);
  case 10:
    cmp[0] = 0;
    cmp[1] = 2;
    return cpsrCheck(state, 9, cmp, 2);
  case 11:
    cmp[0] = 1;
    return cpsrCheck(state, 9, cmp, 1);
  case 12:
    cmp[0] = 1;
    cmp[1] = 3;
    return cpsrCheck(state, 13, cmp, 2);
  case 13:
    cmp[0] = 1;
    cmp[1] = 2;
    cmp[2] = 3;
    return cpsrCheck(state, 13, cmp, 3);
  default:
    perror("ERROR: invalid CPSR code");
    return 0;
  }
}
