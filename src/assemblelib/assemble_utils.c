#include <stdint.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "../combinedlib/combined_utils.h"
#include "assemble_utils.h"
#include "tree.h"

#define POS(x) (x - 'a')

/*
const char *DATA_PROCESSING[15] = {"and", "eor", "sub", "rsb", "add", "orr",
				   "mov", "tst", "teq", "cmp", "lsl"};
const char *MULTIPLY[5] = {"mul", "mla"};
const char *DATA_TRANSFER[5] = {"str", "ldr"};
const char *BRANCH[5] = {"b"};
*/

// set from bit to given value? not sure if a good idea to refactor like that
//however it would be more versatile
static Instruction setBits(uint32_t value, int bit, Instruction instr) {
  return instr |= (value << bit);;
}


// all of these probably take char *nextInstruction or nothing at all
static Instruction setDataProcessing() {

  Instruction instruction = 0;

  //sets Cond Code
  instruction = setBits(al, 31, instruction);

  //if instruction is supposed to be tst, teq or cmp - set S - bit 20
  // otherwise it is clear (cleared upon initialisation)

  return instruction;
}

static Instruction setMultiply() {
  Instruction instruction = 0;
  //sets Cond Code
  instruction = setBits(al, 31, instruction);
  instruction = setBits(0x9, 7, instruction);

  //check whether mnemonic is mla or mul
  // and set A accordingly
  //on initialisation instruction is set with all bits to 0
  // so there is no need to manually set S


  return instruction;
}

static Instruction setDataTransfer() {

  Instruction instruction = 0;
  
  instruction = setBits(1, 26, instruction);
  
  return 0;
}

static Instruction setBranch() {

  Instruction  instruction = 0;
  //check the mnemonic suffix for b and execute with that condition
  //as of now set to al(if no suffix - al)
  //sets Cond Code
  instruction = setBits(al, 31, instruction);
  //sets the 101 from 27
  instruction = setBits(0x5, 27, instruction);

  return instruction;
}

static Instruction setHalt() {
  return 0;
}

int contains(char *value, const char **array){
  for(int i = 0; i < sizeof(array)/sizeof(array[0]); i++){
    if(!strcmp(array[i], value))
      return 1;
  }
  return 0;
}

/*
Instruction assemble(struct Dictionary *symbolTable, const char *nextInstruction) {
    char *opcode;
    char *rest = nextInstruction;


    opcode = strtok_r(rest, " ", &rest);
    if(contains(opcode, DATA_PROCESSING)){
    return setDataProcessing(nextInstruction);
    } else if(contains(opcode, MULTIPLY)){
    return setMultiply();
    } else if(contains(opcode, DATA_TRANSFER)){
    return setDataTransfer();
    } else{
    return setBranch();
    }
  

  return 0;
}
*/

char *getProgramError(errorCode e) {
  errorType errors[2];
  errors[INVALID_INSTRUCTION].code = INVALID_INSTRUCTION;
  errors[INVALID_INSTRUCTION].message = "Invalid instruction";
  //if we have more program error types add them above
  return errors[e].message;
}
