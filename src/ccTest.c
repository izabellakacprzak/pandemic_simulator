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

enum cpsrMask {
  Z = 0b0100,
  NV = 0b1001,
  NZV = 0b1101
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
  int mask = (1 << 32) - (1 << 28);
  int masked = instruction & mask;
  int cmp[3];
  int cmpLength;
  enum cpsrMask newMask;
  enum cpsrMask temp = NV;
  switch (masked) {
  case 0b0000:
    cmp[0] = 1; //Z == 1
    cmpLength = 1;
    newMask = Z;
    break;
  case 0b0001:
    cmp[0] = 0; //Z == 0
    newMask = Z;
    cmpLength = 1;
    break;
  case 0b1010:
    cmp[0] = 0; //N == 0 && V == 0
    cmp[1] = 2; //N == 1 && V == 1
    newMask = NV;
    cmpLength = 2;
    break;
  case 0b1011:
    cmp[0] = 1; //N == 1 && V == 0 || N == 0 && V == 1
    newMask = NV;
    cmpLength = 1;
    break;
  case 0b1100:
    cmp[0] = 1; //Z == 1 && N == 0 && V == 0
    cmp[1] = 3; //Z == 1 && N == 1 && V == 1
    newMask = NZV;
    cmpLength = 2;
    break;
  case 0b1101:
    cmp[0] = 1; //Z == 0 && N == 1 && V == 0 || Z == 0 && N == 0 && V == 1 || Z == 1 && N == 0 && V == 0
    cmp[1] = 2; //Z == 1 && N == 1 && V == 0 || Z == 1 && N == 0 && V == 1
    cmp[2] = 3; // Z == 1 && N == 1 && V == 1
    newMask = NZV;
    cmpLength = 3;
    if ((state.regCPSR & newMask) == (state.regCPSR & temp)) {
      return 0; //Z == 0 && N == 1 && V == 1
    }
    break;
  case 0b1110:
    return 1;
  default:
    perror("ERROR: invalid CPSR code");
    return 0;
  }
  return cpsrCheck(state,newMask,cmp,cmpLength);
}
