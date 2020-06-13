#include <stdint.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>


#include "../combinedlib/combined_utils.h"
#include "assemble_utils.h"
#include "tree.h"

// set at bit to given value
static Instruction setBits(uint32_t value, int bit, Instruction instr) {
  return instr |= (value << bit);;
}


static int calculateOperand(Instruction *instruction, char **code, int opIndex, int processing){
  int operand;

  if(code == NULL){
    return INVALID_INPUT;
  }

  // if operand is an immediate value
  if(code[opIndex][0] == '#'){
    // set the I flag if processing
    *instruction = setBits(processing, 25, *instruction);

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

    // set the I flag if single transfer
    *instruction = setBits(!processing, 25, *instruction);
    
    int rm = atoi(strtok(code[opIndex], "r-+"));
    // set bits 3 - 0 to the Rm register
    *instruction = setBits(rm, 0, *instruction);

    if (!code[opIndex + 1]) {
      //argument 3 is a register with no shift
      return OK;
    }
    
    char *shiftType = code[opIndex + 1];
    char *shiftValue  = code[opIndex + 2];
    
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

	return calculateOperand(instruction, code, 3, 1);
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
	
	return calculateOperand(instruction, code, 2, 1);
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
	
	return calculateOperand(instruction, code, 2, 1);
}


// all of these probably take char *nextInstruction or nothing at all
static int setDataProcessing(Instruction *instruction, char **code, symbolNode *operationNode) {

  //sets condition code
  *instruction = setBits(al, 28, *instruction);

  int output = 0;

  // setting the opcode bits
  //if instruction is supposed to be tst, teq or cmp - set S - bit 20
  // otherwise it is clear (cleared upon initialisation)

  switch (operationNode->data.assemblyLine->code) {
  case AND:
  case EOR:
  case SUB:
  case RSB:
  case ORR:
  case ADD:
    output = withResults(instruction, code); //arithmetic operations
    break;
  case MOV:
    output = operandAssignment(instruction, code);
    break;
  case TST:
  case TEQ:
  case CMP:
    output = noResults(instruction, code);
    break;
  default:
    return INVALID_INPUT;
  }

  *instruction = setBits(operationNode->data.assemblyLine->code, 21, *instruction);

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

static int setMultiply(Instruction *instruction, char **code, symbolNode *operationNode) {

  // sets condition code
  *instruction = setBits(al, 28, *instruction);
  *instruction = setBits(0x9, 4, *instruction);


  // sets the register bits
  // and A bit if instructino is mla
  // there is no need to set S because instruction is initially all 0
  switch(operationNode->data.assemblyLine->code){
  case MUL:
    return setRegistersMultiply(instruction, code);
    break;
  case MLA:
    // if invalid input (no register)
    // output INVALID_INPUT error
    if(code[4][0] != 'r')
      return INVALID_INPUT;
    *instruction = setBits(1, 21, *instruction);
    int rn = atoi(strtok(code[4], "r"));
    *instruction = setBits(rn, 12, *instruction);
    return setRegistersMultiply(instruction, code); 
    break;
  default:
    return INVALID_INPUT;
  }

}

// takes an expression with or without [] brackets, returns the number of arguments in brackets
// assumes destTok[MAX_EXPR_IN_BRACKETS][MAX_INSTRUCTION_SIZE]
static int removeBrackets(char **destTok, char *expression) {
  if (expression[0] != '[') {
    return 0;
  }

  char *delims = "[, ]";
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
    *instruction = setBits(MOV, 21, *instruction);
    return operandAssignment(instruction, code);
  }

  if (expression > 0xFFFFFFFF) {
    printf("Invalid address provided");
    return INVALID_INSTRUCTION;
  }

  ldrAddresses->extraInstructions[ldrAddresses->length] = expression;
  ldrAddresses->length++;
  
  Address currentAddress = *(ldrAddresses->currAddress);
  int32_t offset = (ldrAddresses->lastAddress + (ldrAddresses->length * 4)) - (currentAddress + 8);

  char *offsetC = calloc(7, sizeof(char));

  int err = snprintf(offsetC, 6, "#%i", offset);
  
  err = calculateOperand(instruction, &offsetC, 0, 0);

  free(offsetC);

  // set 0xF in place of Rn
  *instruction = setBits(0xF, 16, *instruction);

  return err;
}

// of the form:
//   <ldr/str> Rd,<address> (where <address> is 1-3 tokens)
static int setDataTransfer(Instruction *instruction, char **code, ldrAddresses *ldrAddresses, symbolNode *operationNode) {

  *instruction = 0;
  
  // set condition code and instruction identity bit
  *instruction = setBits(al, 28, *instruction);
  *instruction = setBits(1, 26, *instruction);

  // Set Rd register
  int rd = strtol(strtok(code[1], "r"), NULL, 0);
  *instruction = setBits(rd, 12, *instruction);

  // default: U bit is set (to add offset)
  *instruction = setBits(1, 23, *instruction);

  switch(operationNode->data.assemblyLine->code) {
  case LDR:
    /* code[0] is "ldr" */
    // set the L bit
    *instruction = setBits(1, 20, *instruction);

    if (code[2][0] == '=') {
      /* code[2] is an immediate value (only possible for ldr) */
      // set P bit
      *instruction = setBits(1, 24, *instruction);
      return dataTransferImmediate(ldrAddresses, code, instruction);
      break;
    }
  case STR:
    break;
  default:
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

  if (!code[3]) {
    // Pre-indexing: set P bit
    *instruction = setBits(1, 24, *instruction);
    
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

	  //setting the I bit
	  *instruction = setBits(1, 25, *instruction);

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
	
	err = calculateOperand(instruction, arg2, 1, 0);
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
      err = calculateOperand(instruction, code, 3, 0);

    } else {
      /* Optional:
           code[2] is register address ([Rn]) (unbracketed in arg2[0])
	   code[3] is offset register (Rm) */
      // shift and type set to 0 (code[4] is NULL)
      if (code[3][0] == '-') {
	// unset U bit if negative
	*instruction &= ~(1 << 23);
      }

      //setting the I bit
      *instruction = setBits(1, 25, *instruction);
	  
      // just need to set rm register
      int rm = strtol(strtok(code[3], "-+r"), NULL, 0);
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
    
    err = calculateOperand(instruction, code, 3, 0);
  }

  // setting Rn register
  int rn = strtol(strtok(arg2[0], "r"), NULL, 0);
  *instruction = setBits(rn, 16, *instruction);
  
  free(arg2);
  
  return err;
}

static int setBranch(Instruction *instruction, char **code, ldrAddresses *ldrAddresses, symbolNode *operationNode, symbolNode *symbolTable) {

  *instruction = 0;

  // set the 101 at bits 27 - 25
  *instruction = setBits(5, 25, *instruction);

  // set condition code
  *instruction = setBits(operationNode->data.assemblyLine->conditionCode, 28, *instruction);

  strcat(code[1], ":");
  symbolNode *assemblyOffset = search(symbolTable, code[1]);
  // if offset is a constant then set address to it
  // if it is a label then find the corresponding address

  int32_t offset;
  if(!assemblyOffset){
    // the address is a constant
    offset = strtol(code[1], NULL, 0);
    
  } else {
  // the address is a label
     Address currentAddress = *(ldrAddresses->currAddress);
     offset = (assemblyOffset->data.address) - (currentAddress + 8);
  }
  
  if (offset > 0x1FFFFFF || offset < -0x1FFFFFF) {
    // offset is more than 26 bits
    return INVALID_INSTRUCTION;
  }
  
  //*instruction = setBits((offset & (1 << 31)) >> 31, 24, *instruction);
  
  offset >>= 2;

  offset = offset & ((1 << 24) - 1);

  *instruction = setBits(offset, 0, *instruction);

  return OK;
}

static int setHalt(Instruction *instruction) {
	instruction = 0;
	return 0;
}

static int setSpecialInstruction(Instruction *instruction, char **code, symbolNode *operationNode){
	code[0] = "mov";
	*code[4] = *code[2];
	*code[2] = *code[1];
	code[3] = "lsl";
	return setDataProcessing(instruction, code, operationNode);
}

int assemble(Instruction *setInstruction, symbolNode *symbolTable, char **nextInstruction, ldrAddresses *ldrAddresses) {
  symbolNode *operationNode = search(symbolTable, nextInstruction[0]);      

  if(operationNode->isLabel){
    return NOT_INSTRUCTION;
  }
  
  InstructionType type = operationNode->data.assemblyLine->type;

  switch(type){
  case BRANCH:
    return setBranch(setInstruction, nextInstruction, ldrAddresses, operationNode, symbolTable);
    break;
  case DATA_PROCESSING:
    return setDataProcessing(setInstruction, nextInstruction, operationNode);
    break;
  case DATA_TRANSFER:
    return setDataTransfer(setInstruction, nextInstruction, ldrAddresses, operationNode);
    break;
  case MULTIPLY:
    return setMultiply(setInstruction, nextInstruction, operationNode);
    break;
  case HALT:
    return setHalt(setInstruction);
    break;
  case SHIFT:
    return setSpecialInstruction(setInstruction, nextInstruction, operationNode);
    break;
  default:
    return INVALID_INSTRUCTION;
  }
}

char *getProgramError(errorCode e) {
  errorType errors[SYS + 1];
  errors[INVALID_INSTRUCTION].code = INVALID_INSTRUCTION;
  errors[INVALID_INSTRUCTION].message = "Invalid instruction";
  
  errors[INVALID_SHIFT].code = INVALID_SHIFT;
  //Is there a better error message?  
  errors[INVALID_SHIFT].message = "Invalid shift";
  
  errors[INVALID_INPUT].code = INVALID_INPUT;
  errors[INVALID_INPUT].message = "Invalid input";

  errors[NOT_INSTRUCTION].code = NOT_INSTRUCTION;
  errors[NOT_INSTRUCTION].message = "Is a label, not an instruction";

  errors[WHITESPACE_LINE].code = WHITESPACE_LINE;
  errors[WHITESPACE_LINE].message = "Is a whitespace line";
  
  errors[OUT_OF_MEMORY].code = OUT_OF_MEMORY;
  errors[OUT_OF_MEMORY].message = "Not enough memory to store current instruction";

  errors[END_OF_FILE].code = END_OF_FILE;
  errors[END_OF_FILE].message = "End of file";

  errors[NULL_FILE].code = NULL_FILE;
  errors[NULL_FILE].message = "File does not exist";
  
  return errors[e].message;
}

