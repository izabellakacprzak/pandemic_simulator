#include <stdint.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>


#include "../combinedlib/combined_utils.h"
#include "assemble_utils.h"
#include "tree.h"

/*
#define POS(x) (x - 'a')

const char *DATA_PROCESSING[15] = {"and", "eor", "sub", "rsb", "add", "orr",
				   "mov", "tst", "teq", "cmp", "lsl"};
const char *MULTIPLY[5] = {"mul", "mla"};
const char *DATA_TRANSFER[5] = {"str", "ldr"};
const char *BRANCH[5] = {"b"};
*/

// set at bit to given value
static Instruction setBits(uint32_t value, int bit, Instruction instr) {
  return instr |= (value << bit);;
}


static int calculateOperand(Instruction *instruction, char **code, int opIndex){
	int operand;

	if(code == NULL){
		return INVALID_INPUT;
	}

	// if operand is an immediate value
	if(code[opIndex][0] == '#'){
		// set the I flag
		*instruction = setBits(1, 25, *instruction);

		operand = strtol(strtok(code[opIndex], "#-+"), NULL, 0);
	
    int rotations = 0;
    while(rotations <= 32 && (operand >> 8) != 0){
      rotations++;
      operand = rol(operand, 2);
    }

    // can not represent constant
    // either because it is too wide
    // or because the number of rotations required is odd
    if(rotations == 32){
      return INVALID_INPUT;
    }
    *instruction = setBits(rotations, 8, *instruction);
    *instruction = setBits(operand, 0, *instruction);
  }
  // if operand is a shifter register
  else{
    if(code[opIndex][0] != 'r'){
      return INVALID_INPUT;
    }
    int rm = atoi(strtok(code[opIndex], "r-+"));
    // set bits 3 - 0 to the Rm register
    *instruction = setBits(rm, 0, *instruction);

    char *shiftValue  = NULL;
    char *shiftType = strtok_r(code[opIndex + 1], " ", &shiftValue);
    // set the shift type bits (6 - 5)
    if(!strcmp(shiftType, "lsl")){
      // the code is 00 so do nothing
    }
    else if(!strcmp(shiftType, "lsr")){
      *instruction = setBits(LSR, 5, *instruction);
    }
    else if(!strcmp(shiftType, "asr")){
      *instruction = setBits(ASR, 5, *instruction);
    }
    else if(!strcmp(shiftType, "ror")){
      *instruction = setBits(ROR, 5, *instruction);
    }
    else{
      // undefined shift
      return INVALID_SHIFT;
    }

    // shiftin by a constant
    if(shiftValue[0] == '#'){
      // bit 4 is 0
      // set bits 11 - 7 to the constant value by which we shift
      *instruction = setBits(strtol(strtok(shiftValue, "#"), NULL, 0), 7, *instruction);
    }
    // shifting by a register
    else if(shiftValue[0] == 'r'){
      // bit 4 is 1
      *instruction = setBits(1, 4, *instruction);
      // set bits 11 - 7 to the register
      *instruction = setBits(atoi(strtok(shiftValue, "r")), 7, *instruction);
    }
    // the amount by which to shift
    // is neither a constant nor a register - invalid
    else{
      return INVALID_INPUT;
    }
  }

  return OK;
}

// sets bits for instructions that
// compute results: and, eor, sub, rsb, add, orr
static int withResults(Instruction *instruction, char **code){
	int rd = atoi(strtok(code[1], "r"));
	int rn = atoi(strtok(code[2], "r"));

	// sets the operand field
	// passing 3 as the operand in the code starts at string at index 3

	*instruction = setBits(rd, 12, *instruction);
	*instruction = setBits(rn , 16, *instruction);

	return calculateOperand(instruction, code, 3);
}

// sets bits for instructions that
// do single operand assignment: mov
static int operandAssignment(Instruction *instruction, char **code){
	if(code[1][0] != 'r'){
		return INVALID_INPUT;
	}
	int rd = atoi(strtok(code[1], "r"));
	
	// sets the operand field
        // passing 2 as the operand in the code starts at string at index 2
		
	*instruction = setBits(rd, 12, *instruction);
	
	return calculateOperand(instruction, code, 2);
}


// sets bits for instructions that
// do not compute results but
// set the CPSR flags: tst, teq, cmp
static int noResults(Instruction *instruction, char **code){
	if(code[1][0] != 'r'){
		return INVALID_INPUT;
	}
	int rn = atoi(strtok(code[1], "r"));
	
	// sets the operand field
        // passing 2 as the operand in the code starts at string at index 2

	*instruction = setBits(rn, 16, *instruction);
	*instruction = setBits(1, 20, *instruction);
	
	return calculateOperand(instruction, code, 2);
}


// all of these probably take char *nextInstruction or nothing at all
static int setDataProcessing(Instruction *instruction, char **code) {

  //sets condition code
  *instruction = setBits(al, 28, *instruction);
  int isWithResults = 0;
  int isOperandAssignment = 0;

  int output = 0;

  // setting the opcode bits
  //if instruction is supposed to be tst, teq or cmp - set S - bit 20
  // otherwise it is clear (cleared upon initialisation)
  if(!strcmp(code[0], "and")){
	  isWithResults = 1;
  }
  else if(!strcmp(code[0], "eor")){
	  isWithResults = 1;
	  *instruction = setBits(eor, 21, *instruction);
  }
  else if(!strcmp(code[0], "sub")){
	  isWithResults = 1;
	  *instruction = setBits(sub, 21, *instruction);
  }
  else if(!strcmp(code[0], "rsb")){
	  isWithResults = 1;
	  *instruction = setBits(rsb, 21, *instruction);
  }
  else if(!strcmp(code[0], "add")){
	  isWithResults = 1;
	  *instruction = setBits(add, 21, *instruction);
  }
  else if(!strcmp(code[0], "orr")){
	  isWithResults = 1;
	  *instruction = setBits(orr, 21, *instruction);
  }
  else if(!strcmp(code[0], "mov")){
	  isOperandAssignment = 1;
	  *instruction = setBits(mov, 21, *instruction);
  }
  else if(!strcmp(code[0], "tst")){
	  *instruction = setBits(tst, 21, *instruction); 
  }
  else if(!strcmp(code[0], "teq")){
	  *instruction = setBits(teq, 21, *instruction);
  }
  else if(!strcmp(code[0], "cmp")){
	  *instruction = setBits(cmp, 21, *instruction);
  }
  else{
	  return INVALID_INPUT;
  }

  if(isWithResults){
	  output = withResults(instruction, code);
  }
  else if(isOperandAssignment){
	  output = operandAssignment(instruction, code);
  }
  else{
	  output = noResults(instruction, code);
  }

  return output;
}

static int setRegistersMultiply(Instruction *instruction, char **code){
	// if invalid input (no register)
	// output INVALID_INPUT error
	if(code[1][0] != 'r' || code[2][0] != 'r' || code[3][0] != 'r')
		return INVALID_INPUT;

	int rd = atoi(strtok(code[1], "r"));
	int rm = atoi(strtok(code[2], "r"));
	int rs = atoi(strtok(code[3], "r"));

	*instruction = setBits(rd, 16, *instruction);
	*instruction = *instruction | rm;
	*instruction = setBits(rs, 8, *instruction);

	return OK;
}

static int setMultiply(Instruction *instruction, char **code) {

  // sets condition code
  *instruction = setBits(al, 18, *instruction);
  *instruction = setBits(0x9, 4, *instruction);


  // sets the register bits
  // and A bit if instructino is mla
  // there is no need to set S because instruction is initially all 0
  if(!strcmp(code[0], "mul")){
	  return setRegistersMultiply(instruction, code);
  }
  else if(!strcmp(code[0], "mla")){
	  // if invalid input (no register)
	  // output INVALID_INPUT error
	  if(code[4][0] != 'r')
		  return INVALID_INPUT;
	  *instruction = setBits(1, 21, *instruction);
	  int rn = atoi(strtok(code[4], "r"));
	  *instruction = setBits(rn, 12, *instruction);
	  return setRegistersMultiply(instruction, code); 
  }
  else{
	  return INVALID_INPUT;
  }
}

// takes an expression with or without [] brackets, returns the number of arguments in brackets
// assumes destTok[MAX_EXPR_IN_BRACKETS][MAX_INSTRUCTION_SIZE]
static int removeBrackets(char **destTok, char *expression) {
  if (expression[0] != '[') {
    return 0;
  }

  char *delims = ",]";
  char *token = strtok(expression, delims);

  int i = 0;
  while (token && i <= MAX_EXPR_IN_BRACKETS) {
    destTok[i] = token;
    token = strtok(NULL, delims);
    i++;
  }

  return i;
}

// for ldr Rd,=<expr> type dataTransfer instructions
static int dataTransferImmediate(ldrAddresses *ldrAddresses, char **code, Instruction *instruction) {
  Address expression = (Address) strtol(strtok(code[2], "="), NULL, 0);

  if (expression <= 0xFF) {
    // treat as a mov instruction (condition code is al as well)
    code[2][0] = '#';
    *instruction = setBits(al, 28, 0);
    *instruction = setBits(13, 21, *instruction); // TODO: MOV enum
    return operandAssignment(instruction, code);
  }

  if (expression > 0xFFFFFFFF) {
    printf("Invalid address provided");
    return INVALID_INSTRUCTION;
  }

  ldrAddresses->extraInstructions[ldrAddresses->length] = expression;
  ldrAddresses->length++;
  // TODO: get current location
  Address currentAddress = 0;
  int32_t offset = (ldrAddresses->lastAddress + (ldrAddresses->length * 4)) - (currentAddress + 8);

  char *offsetC = calloc(7, sizeof(char));

  int err = snprintf(offsetC, 6, "#%i", offset);
  
  err = calculateOperand(instruction, &offsetC, 0);

  free(offsetC);

  return err;
}

// of the form:
//   <ldr/str> Rd,<address> (where <address> is 1-3 tokens)
static int setDataTransfer(Instruction *instruction, char **code, ldrAddresses *ldrAddresses) {

  Instruction instructionVal = 0;
  instruction = &instructionVal;
  
  // set condition code and instruction identity bit
  instructionVal = setBits(al, 28, instructionVal);
  instructionVal = setBits(1, 26, instructionVal);

  // Set Rd register
  int rd = strtol(strtok(code[1], "r"), NULL, 0);
  instructionVal = setBits(rd, 12, instructionVal);

  // default: U bit is set (to add offset)
  instructionVal = setBits(1, 23, instructionVal);

  if (!strcmp(code[0], "ldr")) {
    /* code[0] is "ldr" */
    // set the L bit
    instructionVal = setBits(1, 20, instructionVal);

    if (code[2][0] == '=') {
      /* code[2] is an immediate value (only possible for ldr) */
      // set I and P bits
      instructionVal = setBits(1, 25, instructionVal);
      instructionVal = setBits(1, 24, instructionVal);
      return dataTransferImmediate(ldrAddresses, code, instruction);
    }

  } else if (strcmp(code[0], "str")) {
    /* code[0] is not "ldr" or "str" */
    printf("Invalid dataTransfer label.");
    return INVALID_INSTRUCTION;
  }

  /* code[0] is "ldr" or "str" */

  // will contain split code[2], initialised to NULL
  char **arg2 = calloc(MAX_EXPR_IN_BRACKETS * MAX_INSTRUCTION_SIZE, sizeof(char));

  // saves error codes
  int err = OK;
  // declaring vars that may be used
  int32_t offset;
  int rm;
  char *type;
  int shift;

  // TODO: initialise instruction tokens to NULL
  if (!code[3]) {
    // Pre-indexing: set P bit
    instructionVal = setBits(1, 24, instructionVal);
    
    switch (removeBrackets(arg2, code[2])) {
      case 0:
	printf("Invalid dataTransfer formatting");
	return INVALID_INSTRUCTION;

      case 1:
	/* arg2[0] is register address (Rn)
	   offset is 0 */
	// offset is 0 so nothing needs to be set
	// U bit is positive
	break;
	
      case 2:
        if (arg2[1][0] == 'r') {
	  /* Optional:
	       arg2[0] is address register (Rn)
	       arg2[1] is offset register (Rm)
	       shift is 0, type is NULL (arg2[2] is NULL) */
	  if (arg2[1][0] == '-') {
	    // unset U bit if negative
	    *instruction &= ~(1 << 23);
	  }
	  
	  // just need to set rm register
	  int rm = strtol(strtok(arg2[1], "-+r"), NULL, 0);
	  *instruction = setBits(rm, 0, *instruction);
	  break;
	} 

	// all other cases pass to case 3
	/* Possibly:
	     arg2[0] is address register (Rn)
	     arg2[1] is immediate offset */
	
      case 3:
	/* Optional:
	     arg2[0] is register address (Rn)
	     arg2[1] is offset register with sign ({+/-}Rm) (unset U bit (23) if negative)
	     arg2[2] is a shift expression (eg. lsr #2) */
	if (arg2[1][0] == '-' || arg2[1][1] == '-') {
	  // unset U bit if negative
	  *instruction &= ~(1 << 23);
	}
	
	err = calculateOperand(instruction, arg2, 1);
	break;

      default:
	printf("Invalid dataTransfer formatting");
	return INVALID_INSTRUCTION;
      
    } // end of switch
    
  } else if (!code[4]) {
    // Post-indexing (P not set)

    if (removeBrackets(arg2, code[2]) != 1) {
      printf("Invalid dataTransfer formatting");
      return INVALID_INSTRUCTION;
    }
    
    if (code[3][0] == '#') {
      /* code[2] is register address ([Rn]) (unbracketed in arg2[0])
	 code[3] is immediate offset */
      if (code[3][1] == '-') {
	// unset U bit if negative
	*instruction &= ~(1 << 23);
      }
      err = calculateOperand(instruction, code, 3);

    } else {
      /* Optional:
           code[2] is register address ([Rn]) (unbracketed in arg2[0])
	   code[3] is offset register (Rm) */
      // shift and type set to 0 (code[4] is NULL)
      if (code[3][0] == '-') {
	// unset U bit if negative
	*instruction &= ~(1 << 23);
      }
	  
      // just need to set rm register
      int rm = strtol(strtok(arg2[1], "-+r"), NULL, 0);
      *instruction = setBits(rm, 0, *instruction);

    }

  } else {
    /* Optional (Post-indexing (P not set)):
         code[2] is register address (Rn) (unbracketed in arg2[0])
	 code[3] is offset register with sign ({+/-}Rm) (unset U bit (23) if negative)
         code[4] is a shift expression (eg. lsl #2) */

    if (removeBrackets(arg2, code[2]) != 1) {
      printf("Invalid dataTransfer formatting");
      return INVALID_INSTRUCTION;
    }

    if (code[3][0] == '-') {
      // unset U bit if negative
      *instruction &= ~(1 << 23);
    }
    
    err = calculateOperand(instruction, code, 3);
  }

  // setting Rn register
  int rn = strtol(strtok(arg2[0], "r"), NULL, 0);
  instructionVal = setBits(rn, 16, instructionVal);
  
  free(arg2);
  
  return err;
}

static int setBranch(Instruction *instruction, char **code) {

  *instruction = 0;

  // set the 101 at bits 27 - 25
  *instruction = setBits(0x5, 25, *instruction);


  // set condition code
  if(!strcmp(code[0], "bne")){
	  *instruction = setBits(1, 28, *instruction);
  }
  else if(!strcmp(code[0], "bge")){
	  *instruction = setBits(10, 28, *instruction);
  }
  else if(!strcmp(code[0], "blt")){
	  *instruction = setBits(11, 28, *instruction);
  }
  else if(!strcmp(code[0], "bgt")){
	  *instruction = setBits(12, 28, *instruction);
  }
  else if(!strcmp(code[0], "ble")){
	  *instruction = setBits(13, 28, *instruction);
  }
  else if(!strcmp(code[0], "bal") || !strcmp(code[0], "b")){
	  *instruction = setBits(14, 28, *instruction);
  }
  else{
	  return INVALID_INPUT;
  }

  // if offset is a constant then set address to it
  // if it is a label then find the corresponding address

  /*
  int address = find(dict, code[1]);
  // the address in a constant
  if(address == -1){
	  instruction = setBits(strtol(code[1], NULL, 0), 0, instruction);
  }
  // the address is a label
  else{
	  instruction = setBits(address, 0, instruction);
  }
  */


  return 0;
}

static int setHalt(Instruction *instruction) {
	instruction = 0;
	return 0;
}

static int setSpecialInstruction(Instruction *instruction, char **code){
	code[0] = "mov";
	*code[4] = *code[2];
	*code[2] = *code[1];
	code[3] = "lsl";
	return setDataProcessing(instruction, code);
}

int contains(char *value, const char **array){
  for(int i = 0; i < sizeof(array)/sizeof(array[0]); i++){
    if(!strcmp(array[i], value))
      return 1;
  }
  return 0;
}

int assemble(Instruction *setInstruction, symbolNode *symbolTable, char **nextInstruction) {
  symbolNode assemblyInstr = *search(symbolTable, nextInstruction[0]);      
  InstructionType type = assemblyInstr.data.assemblyLine->type;
  if(assemblyInstr.isLabel){
    return 0;
  }

  switch(type){
  case BRANCH:
    return setBranch(setInstruction, nextInstruction);
    break;
  case DATA_PROCESSING:
    return setDataProcessing(setInstruction, nextInstruction);
    break;
  case DATA_TRANSFER:
    return setDataTransfer(setInstruction, nextInstruction, 0);
    break;
  case MULTIPLY:
    return setMultiply(setInstruction, nextInstruction);
    break;
  case HALT:
    return setHalt(setInstruction);
    break;
  default:
    return INVALID_INSTRUCTION;
  }
}

char *getProgramError(errorCode e) {
  errorType errors[SYS + 1];
  errors[INVALID_INSTRUCTION].code = INVALID_INSTRUCTION;
  errors[INVALID_INSTRUCTION].message = "Invalid instruction";
  
  errors[INVALID_INSTRUCTION].code = INVALID_SHIFT;
  //Is there a better error message?  
  errors[INVALID_INSTRUCTION].message = "Invalid shift";
  
  errors[INVALID_INSTRUCTION].code = INVALID_INPUT;
  errors[INVALID_INSTRUCTION].message = "Invalid input";
  
  errors[INVALID_INSTRUCTION].code = OUT_OF_MEMORY;
  errors[INVALID_INSTRUCTION].message = "Not enough memory to store current instruction";

  errors[INVALID_INSTRUCTION].code = END_OF_FILE;
  errors[INVALID_INSTRUCTION].message = "End of file";
  
  return errors[e].message;
}

