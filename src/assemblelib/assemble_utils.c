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

// set from bit to given value? not sure if a good idea to refactor like that
//however it would be more versatile
static Instruction setBits(uint32_t value, int bit, Instruction instr) {
  return instr |= (value << bit);;
}


int calculateOperand(Instruction *instruction, char **code, int opIndex){
	int operand;
	// is operand is an immediate value
	if(code[opIndex][0] == '#'){
		// set the I flag
		*instruction = setBits(1, 25, *instruction);

		operand = strtol(strtok(code[opIndex], "#"), NULL, 0);
		
		int rotations = 0;
		while((operand >> 8) != 0){
			rotations++;
			operand = (operand >> 1) | (operand << 31);
		}
		// not sure if this is correct
		*instruction = setBits(rotations/2, 8, *instruction);
		*instruction = setBits(operand, 0, *instruction);
	}
	// if operand is a shifter register
	else{
		int rm = atoi(strtok(code[opIndex], "r"));
		// set bits 3 - 0 to the Rm register
		*instruction = setBits(rm, 0, *instruction);

		// set the shift type bits (6 - 5)
		if(!strcmp(code[opIndex + 1], "lsl")){
			// the code is 00 so do nothing
		}
		else if(!strcmp(code[opIndex + 1], "lsr")){
			*instruction = setBits(1, 5, *instruction);
		}
		else if(!strcmp(code[opIndex + 1], "asr")){
			*instruction = setBits(2, 5, *instruction);
		}
		else if(!strcmp(code[opIndex + 1], "ror")){
			*instruction = setBits(3, 5, *instruction);
		}
		else{
			// undefined shift
			return INVALID_SHIFT;
		}

		// shiftin by a constant
		if(code[opIndex + 2][0] == '#'){
			// bit 4 is 0
			// set bits 11 - 7 to the constant value by which we shift
			*instruction = setBits(strtol(strtok(code[opIndex + 2], "#"), NULL, 0), 7, *instruction);
		}
		// shifting by a register
		else{
			// bit 4 is 1
			*instruction = setBits(1, 4, *instruction);
			// set bits 11 - 7 to the register
			*instruction = setBits(atoi(strtok(code[opIndex + 2], "r")), 7, *instruction);
		}
	}

	return OK;
}

// sets bits for instructions that
// compute results: and, eor, sub, rsb, add, orr
int withResults(Instruction *instruction, char **code){
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
int operandAssignment(Instruction *instruction, char **code){
	int rd = atoi(strtok(code[1], "r"));
	
	// sets the operand field
        // passing 2 as the operand in the code starts at string at index 2
		
	*instruction = setBits(rd, 12, *instruction);
	
	return calculateOperand(instruction, code, 2);
}


// sets bits for instructions that
// do not compute results but
// set the CPSR flags: tst, teq, cmp
int noResults(Instruction *instruction, char **code){
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

int setRegistersMultiply(Instruction *instruction, char **code){
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

static int dataTransferImmediate(Instruction *instruction, char **code) {
  Address expression = (Address) strtol(strtok(code[2], "="), NULL, 0);

  if (expression <= 0xFF) {
    // treat as a mov instruction (condition code is al as well)
    code[2][0] = '#';
    *instruction = setBits(al, 28, 0);
    *instruction = setBits(13, 21, *instruction);
    return OK; // operandAssignment(instruction, code);
  }

  if (expression > 0xFFFFFFFF) {
    printf("Invalid address provided");
    return INVALID_INSTRUCTION;
  }

  // TODO:
  // needs to place expression at the end of the assembled code and
  //   use its memory position to calculate the offset
  printf("DataTransfer immediate values not yet implemented");
  return OK;
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

static int setOffsetImmediate(int32_t offset, Instruction *instruction) {
  // set I flag
  *instruction = setBits(1, 25, *instruction);

  if (offset == 0) {
    return OK;
  }

  if (offset < 0) {
    // remove U bit if negative
    *instruction &= ~(1 << 23);
    offset = -offset;
  }

  // TODO: implement for non-zero offset (possibly with withResult())

  return OK;
}

// Optional:
//   shift is always a constant
static int setOffsetRegister(int shift, const char *type, int rm, Instruction *instruction) {
  // set Rm register
  *instruction = setBits(rm, 0, *instruction);

  if (shift < 0) {
    // unset U bit if negative
    *instruction &= ~(1 << 23);
    shift = -shift;
  }

  // bit 4 (shift by specified register) is not set, as shift is a constant
  *instruction = setBits(shift, 7, *instruction);

  // TODO: use type to set shift bits 6-5
  if (!type) {
    printf("Invalid dataTransfer formatting");
    return INVALID_INSTRUCTION;
    
  } else if (!strcmp(type, "lsl")) {
    *instruction = setBits(lsl_c, 5, *instruction);
    
  } else if (!strcmp(type, "lsr")) {
    *instruction = setBits(lsr_c, 5, *instruction);
    
  } else if (!strcmp(type, "asr")) {
    *instruction = setBits(asr_c, 5, *instruction);
    
  } else if (!strcmp(type, "ror")) {
    *instruction = setBits(ror_c, 5, *instruction);
    
  } else {
    return INVALID_INSTRUCTION;
  }

  return OK;
}

// of the form:
//   <ldr/str> Rd, <address> (where <address> is 1-3 tokens)
static int  setDataTransfer(Instruction *instruction, char **code, ldrAddresses *ldrAddresses) {

  Instruction instructionVal = 0;
  instruction = &instructionVal;

  // saves error codes
  int err = OK;
  
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
      instructionVal = setBits(1, 25, instructionVal);
      err = dataTransferImmediate(instruction, code);
    }

  } else if (strcmp(code[0], "str")) {
    /* code[0] is not "ldr" or "str" */
    printf("Invalid dataTransfer label.");
    return INVALID_INSTRUCTION;
  }

  /* code[0] is "ldr" or "str" */

  // will contain split code[2]
  char **arg2 = calloc(MAX_EXPR_IN_BRACKETS * MAX_INSTRUCTION_SIZE, sizeof(char));

  // declaring vars that may be used
  int32_t offset;
  char *type;
  int shift;
  int rm;

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
	
	err = setOffsetImmediate(0, instruction);
	
      case 2:
	if (arg2[1][0] == '#') {
	  /* arg2[0] is address register (Rn)
	     arg2[1] is immediate offset */
	  
	  offset = strtol(strtok(arg2[1], "#"), NULL, 0);
	  err = setOffsetImmediate(offset, instruction);
	  
	} else if (arg2[1][0] == 'r') {
	  /* Optional:
	       arg2[0] is address register (Rn)
	       arg2[1] is offset register (Rm) */
	  
	  rm = strtol(strtok(arg2[1], "r"), NULL, 0);
	  // shift set to 0 and type set to NULL
	  err = setOffsetRegister(0, NULL, rm, instruction);
	  
	} else {
	  printf("Invalid dataTransfer formatting");
	  return INVALID_INSTRUCTION;
	}
	
      case 3:
	/* Optional:
	     arg2[0] is register address (Rn)
	     arg2[1] is offset register with sign ({+/-}Rm) (unset U bit (23) if negative)
	     arg2[2] is a shift expression (eg. lsr #2) */
	type = strtok(arg2[2], " #");
	shift = strtol(strtok(NULL, " #"), NULL, 0);
	
	if (arg2[1][0] == '-') {
	  shift = -shift;
	}

	rm = strtol(strtok(arg2[1], "-+r"), NULL, 0);
	err = setOffsetRegister(shift, type, rm, instruction);

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
      
      offset = strtol(strtok(code[3], "#"), NULL, 0);
      err = setOffsetImmediate(offset, instruction);

    } else if (code[3][0] == 'r') {
      /* Optional:
           code[2] is register address ([Rn]) (unbracketed in arg2[0])
	   code[3] is offset register (Rm) */

      rm = strtol(strtok(code[3], "r"), NULL, 0);
      // shift and type set to 0
      err = setOffsetRegister(0, NULL, rm, instruction);

    } else {
      printf("Invalid dataTransfer formatting");
      return INVALID_INSTRUCTION;
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
    
    type = strtok(code[4], " #");
    shift = strtol(strtok(NULL, " #"), NULL, 0);
	
    if (code[3][0] == '-') {
      shift = -shift;
    }

    rm = strtol(strtok(code[3], "-+r"), NULL, 0);
    err = setOffsetRegister(shift, type, rm, instruction);
  }

  // setting Rn register
  int rn = strtol(strtok(arg2[0], "r"), NULL, 0);
  instructionVal = setBits(rn, 16, instructionVal);
  
  free(arg2);
  
  return err;
}

static Instruction setBranch(char **code) {

  Instruction  instruction = 0;

  // set the 101 at bits 27 - 25
  instruction = setBits(0x5, 25, instruction);


  // set condition code
  if(!strcmp(code[0], "bne")){
	  instruction = instruction | (1 << 28);
  }
  else if(!strcmp(code[0], "bge")){
	  instruction = instruction | (10 << 28);
  }
  else if(!strcmp(code[0], "blt")){
	  instruction = instruction | (11 << 28);
  }
  else if(!strcmp(code[0], "bgt")){
	  instruction = instruction | (12 << 28);
  }
  else if(!strcmp(code[0], "ble")){
	  instruction = instruction | (13 << 28);
  }
  else{
	  instruction = instruction | (14 << 28);
  }

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

Instruction assemble(symbolNode *symbolTable, const char *nextInstruction) {
  /*
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
  */

  return 0;
}

char *getProgramError(errorCode e) {
  errorType errors[2];
  errors[INVALID_INSTRUCTION].code = INVALID_INSTRUCTION;
  errors[INVALID_INSTRUCTION].message = "Invalid instruction";
  //if we have more program error types add them above
  return errors[e].message;
}

