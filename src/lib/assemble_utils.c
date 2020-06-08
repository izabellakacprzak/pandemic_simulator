#include <stdint.h>
#include "combined_utils.h"
#include "assemble_utils.h"

// takes current instruction and sets a register at bit,
//   or just takes bit and can be used as a mask
static Instruction setRegister(int regNum, int bit) {
  return 0;
}

// all of these probably take char *nextInstruction or nothing at all
static Instruction setDataProcessing(char *nextInstruction, ) {
  return 0;
}

static Instruction setMultiply() {
  return 0;
}

static Instruction setDataTransfer() {
  return 0;
}

static Instruction setBranch() {
  return 0;
}

static Instruction setHalt() {
  return 0;
}

Instruction assemble(Dictionary *symbolTable, char *nextInstruction) {
  return 0;
}
