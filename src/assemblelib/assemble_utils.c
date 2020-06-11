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


void calculateOperand(Instruction instruction, char **code, int opIndex){
	int operand;
	// is operand is an immediate value
	if(code[opIndex][0] == '#'){
		// set the I flag
		instruction = setBits(1, 25, instruction);

		operand = strtol(strtok(code[opIndex], "#"), NULL, 0);
		
		int rotations = 0;
		while((operand >> 8) != 0){
			rotations++;
			operand = (operand >> 1) | (operand << 31);
		}
		// not sure if this is correct
		instruction = setBits(rotations/2, 8, instruction);
		instruction = setBits(operand, 0, instruction);
	}
	// if operand is a shifter register
	else{
		int rm = atoi(strtok(code[opIndex], "r"));
		// set bits 3 - 0 to the Rm register
		instruction = setBits(rm, 0, instruction);

		// set the shift type bits (6 - 5)
		if(!strcmp(code[opIndex + 1], "lsl")){
			// the code is 00 so do nothing
		}
		else if(!strcmp(code[opIndex + 1], "lsr")){
			instruction = setBits(1, 5, instruction);
		}
		else if(!strcmp(code[opIndex + 1], "asr")){
			instruction = setBits(2, 5, instruction);
		}
		else if(!strcmp(code[opIndex + 1], "ror")){
			instruction = setBits(3, 5, instruction);
		}
		else{
			// undefined shift ??
		}

		// shiftin by a constant
		if(code[opIndex + 2][0] == '#'){
			// bit 4 is 0
			// set bits 11 - 7 to the constant value by which we shift
			instruction = setBits(strtol(strtok(code[opIndex + 2], "#"), NULL, 0), 7, instruction);
		}
		// shifting by a register
		else{
			// bit 4 is 1
			instruction = setBits(1, 4, instruction);
			// set bits 11 - 7 to the register
			instruction = setBits(atoi(strtok(code[opIndex + 2], "r")), 7, instruction);
		}
	}
}

// sets bits for instructions that
// compute results: and, eor, sub, rsb, add, orr
int withResults(Instruction instruction, char **code){
	int rd = atoi(strtok(code[1], "r"));
	int rn = atoi(strtok(code[2], "r"));

	// sets the operand field
	// passing 3 as the operand in the code starts at string at index 3
	calculateOperand(instruction, code, 3);

	instruction = setBits(rd, 12, instruction);
	instruction = setBits(rn , 16, instruction);

	return 0;
}

// sets bits for instructions that
// do single operand assignment: mov
void operandAssignment(Instruction instruction, char **code){
	int rd = atoi(strtok(code[1], "r"));
	
	// sets the operand field
        // passing 2 as the operand in the code starts at string at index 2
	calculateOperand(instruction, code, 2);
	instruction = setBits(rd, 12, instruction);
}


// sets bits for instructions that
// do not compute results but
// set the CPSR flags: tst, teq, cmp
void noResults(Instruction instruction, char **code){
	int rn = atoi(strtok(code[1], "r"));
	
	// sets the operand field
        // passing 2 as the operand in the code starts at string at index 2
	calculateOperand(instruction, code, 2);

	instruction = setBits(rn, 16, instruction);

	instruction = setBits(1, 20, instruction);
}


// all of these probably take char *nextInstruction or nothing at all
static Instruction setDataProcessing(char **code) {

  Instruction instruction = 0;

  //sets condition code
  instruction = setBits(al, 28, instruction);
 
  // setting the opcode bits
  //if instruction is supposed to be tst, teq or cmp - set S - bit 20
  // otherwise it is clear (cleared upon initialisation)
  if(!strcmp(code[0], "and")){
	  withResults(instruction, code);
  }
  else if(!strcmp(code[0], "eor")){
	  withResults(instruction, code);
	  instruction = setBits(1, 21, instruction);
  }
  else if(!strcmp(code[0], "sub")){
	  withResults(instruction, code);
	  instruction = setBits(2, 21, instruction);
  }
  else if(!strcmp(code[0], "rsb")){
	  withResults(instruction, code);
	  instruction = setBits(3, 21, instruction);
  }
  else if(!strcmp(code[0], "add")){
	  withResults(instruction, code);
	  instruction = setBits(4, 21, instruction);
  }
  else if(!strcmp(code[0], "orr")){
	  withResults(instruction, code);
	  instruction = setBits(12, 21, instruction);
  }
  else if(!strcmp(code[0], "mov")){
	  operandAssignment(instruction, code);
	  instruction = setBits(13, 21, instruction);
  }
  else if(!strcmp(code[0], "tst")){
	  noResults(instruction, code);
	  instruction = setBits(8, 21, instruction); 
  }
  else if(!strcmp(code[0], "teq")){
	  noResults(instruction, code);
	  instruction = setBits(9, 21, instruction);
  }
  else if(!strcmp(code[0], "cmp")){
	  noResults(instruction, code);
	  instruction = setBits(10, 21, instruction);
  }

  return instruction;
}

void setRegistersMultiply(Instruction instruction, char **code){
	int rd = atoi(strtok(code[1], "r"));
	int rm = atoi(strtok(code[2], "r"));
	int rs = atoi(strtok(code[3], "r"));

	instruction = setBits(rd, 16, instruction);
	instruction = instruction | rm;
	instruction = setBits(rs, 8, instruction);
}

static Instruction setMultiply(char **code) {
  Instruction instruction = 0;

  // sets condition code
  instruction = setBits(al, 18, instruction);
  instruction = setBits(0x9, 4, instruction);


  // sets the register bits
  // and A bit if instructino is mla
  // there is no need to set S because instruction is initially all 0
  if(!strcmp(code[0], "mul")){
	  setRegistersMultiply(instruction, code);
  }
  else if(!strcmp(code[0], "mla")){
	  instruction = setBits(1, 21, instruction);
	  setRegistersMultiply(instruction, code);
	  int rn = atoi(strtok(code[4], "r"));
	  instruction = setBits(rn, 12, instruction);
  }

  return instruction;
}

static Instruction dataTransferImmediate(Instruction instruction, char **code) {
  Address expression = (Address) strtol(strtok(code[2], "="), NULL, 0);

  if (expression <= 0xFF) {
    // treat as a mov instruction (condition code is al as well)
    code[2][0] = '#';
    instruction = setBits(al, 28, 0);
    operandAssignment(instruction, code);
    setBits(13, 21, instruction);
    return instruction;
  }

  if (expression > 0xFFFFFFFF) {
    printf("Invalid address provided");
    return instruction;
  }

  // TODO:
  // needs to place expression at the end of the assembled code and
  //   use its memory position to calculate the offset
  printf("DataTransfer immediate values not yet implemented");
  return instruction;
}

#define MAX_EXPR_IN_BRACKETS 3
#define MAX_INSTRUCTION_SIZE 511

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

static Instruction setOffsetImmediate(int32_t offset, Instruction instruction) {
  // set I flag
  instruction = setBits(1, 25, instruction);

  if (offset == 0) {
    return instruction;
  }

  if (offset < 0) {
    // remove U bit if negative
    instruction &= ~(1 << 23);
    offset = -offset;
  }

  // TODO: implement for non-zero offset (possibly with withResult())

  return instruction;
}

// Optional:
//   shift is always a constant
static Instruction setOffsetRegister(int shift, const char *type, int rm, Instruction instruction) {
  // set Rm register
  instruction = setBits(rm, 0, instruction);

  if (shift < 0) {
    // unset U bit if negative
    instruction &= ~(1 << 23);
    shift = -shift;
  }

  // bit 4 (shift by specified register) is not set, as shift is a constant
  instruction = setBits(shift, 7, instruction);

  // TODO: use type to set shift bits 6-5
  if (!type) {
    return instruction;
  }

  return instruction;
}

// of the form:
//   <ldr/str> Rd, <address> (where <address> is 1-3 tokens)
static Instruction setDataTransfer(/* symbolNode * ,*/char **code) {

  Instruction instruction = 0;
  
  // set condition code and instruction identity bit
  instruction = setBits(al, 28, instruction);
  instruction = setBits(1, 26, instruction);

  // Set Rd register
  int rd = strtol(strtok(code[1], "r"), NULL, 0);
  instruction = setBits(rd, 12, instruction);

  // default: U bit is set (to add offset)
  instruction = setBits(1, 23, instruction);

  if (!strcmp(code[0], "ldr")) {
    /* code[0] is "ldr" */
    // set the L bit
    instruction = setBits(1, 20, instruction);

    if (code[2][0] == '=') {
      /* code[2] is an immediate value (only possible for ldr) */
      instruction = setBits(1, 25, instruction);
      return dataTransferImmediate(instruction, code);
    }

  } else if (strcmp(code[0], "str")) {
    /* code[0] is not "ldr" or "str" */
    printf("Invalid dataTransfer label.");
    return instruction;
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
    instruction = setBits(1, 24, instruction);
    
    switch (removeBrackets(arg2, code[2])) {
      case 0:
	printf("Invalid dataTransfer formatting");
	return instruction;

      case 1:
	/* arg2[0] is register address (Rn)
	   offset is 0 */
	
	setOffsetImmediate(0, instruction);
	break;
	
      case 2:
	if (arg2[1][0] == '#') {
	  /* arg2[0] is address register (Rn)
	     arg2[1] is immediate offset */
	  
	  offset = strtol(strtok(arg2[1], "#"), NULL, 0);
	  setOffsetImmediate(offset, instruction);
	  
	} else if (arg2[1][0] == 'r') {
	  /* Optional:
	       arg2[0] is address register (Rn)
	       arg2[1] is offset register (Rm) */
	  
	  rm = strtol(strtok(arg2[1], "r"), NULL, 0);
	  // shift set to 0 and type set to NULL
	  instruction = setOffsetRegister(0, NULL, rm, instruction);
	  
	} else {
	  printf("Invalid dataTransfer formatting");
	  return instruction;
	}
	
	break;
	
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

	instruction = setOffsetRegister(shift, type, rm, instruction);

	break;
      default:
	printf("Invalid dataTransfer formatting");
      
    } // end of switch
    
  } else if (!code[4]) {
    // Post-indexing (P not set)

    if (removeBrackets(arg2, code[2]) != 1) {
      printf("Invalid dataTransfer formatting");
      return instruction;
    }
    
    if (code[3][0] == '#') {
      /* code[2] is register address ([Rn]) (unbracketed in arg2[0])
	 code[3] is immediate offset */
      
      offset = strtol(strtok(code[3], "#"), NULL, 0);
      setOffsetImmediate(offset, instruction);

    } else if (code[3][0] == 'r') {
      /* Optional:
           code[2] is register address ([Rn]) (unbracketed in arg2[0])
	   code[3] is offset register (Rm) */

      rm = strtol(strtok(code[3], "r"), NULL, 0);
      // shift and type set to 0
      instruction = setOffsetRegister(0, NULL, rm, instruction);

    } else {
      printf("Invalid dataTransfer formatting");
    }

  } else {
    /* Optional (Post-indexing (P not set)):
         code[2] is register address (Rn) (unbracketed in arg2[0])
	 code[3] is offset register with sign ({+/-}Rm) (unset U bit (23) if negative)
         code[4] is a shift expression (eg. lsl #2) */

    if (removeBrackets(arg2, code[2]) != 1) {
      printf("Invalid dataTransfer formatting");
      return instruction;
    }
    
    type = strtok(code[4], " #");
    shift = strtol(strtok(NULL, " #"), NULL, 0);
	
    if (code[3][0] == '-') {
      shift = -shift;
    }

    rm = strtol(strtok(code[3], "-+r"), NULL, 0);

    instruction = setOffsetRegister(shift, type, rm, instruction);
  }

  // setting Rn register
  int rn = strtol(strtok(arg2[0], "r"), NULL, 0);
  instruction = setBits(rn, 16, instruction);
  
  free(arg2);
  
  return instruction;
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

Instruction assemble(symbolNode *symbolTable, char **nextInstruction) {
  
  return 0;
}

char *getProgramError(errorCode e) {
  errorType errors[2];
  errors[INVALID_INSTRUCTION].code = INVALID_INSTRUCTION;
  errors[INVALID_INSTRUCTION].message = "Invalid instruction";
  //if we have more program error types add them above
  return errors[e].message;
}

