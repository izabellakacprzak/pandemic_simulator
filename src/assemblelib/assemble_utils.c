#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../combinedlib/combined_utils.h"
#include "assemble_utils.h"
#include "tree.h"

/* Set bits starting from "bit" to a given value */
static Instruction setBits(uint32_t value, int bit, Instruction instr) {
  return instr |= (value << bit);;
}

/* Calculates the second operand in Data Transfer and Data Processing instructions */
static int calculateOperand(Instruction *instruction, char **code, int opIndex, int processing) {
  int operand;

  if(code == NULL) {
    return INVALID_INPUT;
  }

  /* Operand is an immediate value */
  if(code[opIndex][0] == '#') {
    /* Set the I flag if processing */
    *instruction = setBits(processing, 25, *instruction);

    operand = strtol(strtok(code[opIndex], "#-+"), NULL, 0);
	
    int rotations = 0;
    while(rotations <= 32 && (operand >> 8) != 0) {
      rotations++;
      operand = rol(operand, 2);
    }

    /* Cannot represent constant either because it is too wide
       or because the number of rotations required is odd */
    if(rotations == 32) {
      return INVALID_INPUT;
    }
    *instruction = setBits(rotations, 8, *instruction);
    *instruction = setBits(operand, 0, *instruction);    
  } else {
    /* Operand is a shifted register */
    if(code[opIndex][0] != 'r') {
      return INVALID_INPUT;
    }

    /* Set the I flag if single transfer */
    *instruction = setBits(!processing, 25, *instruction);
    
    int rm = atoi(strtok(code[opIndex], "r-+"));
    /* Set bits 3 - 0 to the Rm register */
    *instruction = setBits(rm, 0, *instruction);

    if (!code[opIndex + 1]) {
      /* Argument 3 is a register with no shift */
      return OK;
    }
    
    char *shiftType = code[opIndex + 1];
    char *shiftValue  = code[opIndex + 2];
    
    /* Set the shift type bits (6 - 5) */
    if(!strcmp(shiftType, "lsl")) {
      /* The code is 00 so do nothing */
    }
    else if(!strcmp(shiftType, "lsr")) {
      *instruction = setBits(LSR, 5, *instruction);
    }
    else if(!strcmp(shiftType, "asr")) {
      *instruction = setBits(ASR, 5, *instruction);
    }
    else if(!strcmp(shiftType, "ror")) {
      *instruction = setBits(ROR, 5, *instruction);
    }
    else{
      /* Undefined shift */
      return INVALID_SHIFT;
    }

    /* Shifting by a constant */
    if(shiftValue[0] == '#') {
      /* Bit 4 is 0, set bits 11 - 7 
	 to the constant value by which we shift */
      *instruction = setBits(strtol(strtok(shiftValue, "#"), NULL, 0), 7, *instruction);
    }
    // shifting by a register
    else if(shiftValue[0] == 'r') {
      /* Bit 4 is 1 */
      *instruction = setBits(1, 4, *instruction);
      /* Set bits 11 - 8 to the register */
      *instruction = setBits(atoi(strtok(shiftValue, "r")), 8, *instruction);
    }
    /* The amount by which we shift is neither
       a constant nor a register - invalid */
    else {
      return INVALID_INPUT;
    }
  }

  return OK;
}

/* Set bits for instructions that compute results: 
   and, eor, sub, rsb, add, orr */
static int withResults(Instruction *instruction, char **code) {
	int rd = atoi(strtok(code[1], "r"));
	int rn = atoi(strtok(code[2], "r"));

	*instruction = setBits(rd, 12, *instruction);
	*instruction = setBits(rn , 16, *instruction);
	
	/* Set the operand field
	   passing 3 as the operand as the code starts at string at index 3 */
	return calculateOperand(instruction, code, 3, 1);
}

/* Set bits for instructions that 
   perform single operand assignment: mov */
static int operandAssignment(Instruction *instruction, char **code) {
	if(code[1][0] != 'r') {
		return INVALID_INPUT;
	}
       	int rd = atoi(strtok(code[1], "r"));
      	*instruction = setBits(rd, 12, *instruction);
	
	/* Set the operand field
	   passing 2 as the operand as the code starts at string at index 2 */
	return calculateOperand(instruction, code, 2, 1);
}


/* Set bits for instructions that do not compute results 
   but set the CPSR flags: tst, teq, cmp */
static int noResults(Instruction *instruction, char **code) {
	if(code[1][0] != 'r') {
		return INVALID_INPUT;
	}
	int rn = atoi(strtok(code[1], "r"));	
	*instruction = setBits(rn, 16, *instruction);
	*instruction = setBits(1, 20, *instruction);

	/* Set the operand field
	   passing 2 as the operand as the code starts at string at index 2 */
	return calculateOperand(instruction, code, 2, 1);
}

static int setDataProcessing(Instruction *instruction, char **code, symbolNode *operationNode) {
  /* Set the condition code */
  *instruction = setBits(al, 28, *instruction);

  int output = 0;

  switch(operationNode->data.assemblyLine->code) {
  case AND:
  case EOR:
  case SUB:
  case RSB:
  case ORR:
  case ADD:
    output = withResults(instruction, code);
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

  /* If instruction is tst, teq or cmp - set S - bit 20 */
  *instruction = setBits(operationNode->data.assemblyLine->code, 21, *instruction);

  return output;
}

static int setRegistersMultiply(Instruction *instruction, char **code) {
  /* Check whether the input is valid 
     and the elements of the code are registers */
  if(code[1][0] != 'r' || code[2][0] != 'r' || code[3][0] != 'r') {
    return INVALID_INPUT;
  }
  int rd = atoi(strtok(code[1], "r"));
  int rm = atoi(strtok(code[2], "r"));
  int rs = atoi(strtok(code[3], "r"));

  *instruction = setBits(rd, 16, *instruction);
  *instruction = *instruction | rm;
  *instruction = setBits(rs, 8, *instruction);

  return OK;
}

static int setMultiply(Instruction *instruction, char **code, symbolNode *operationNode) {
  /* Set the condition code */
  *instruction = setBits(al, 28, *instruction);
  *instruction = setBits(0x9, 4, *instruction);

  /* Set the register bits and A bit if 
     instruction is mla - A is clear upon initialisation 
     which is the case when the instruction is mul */
  switch(operationNode->data.assemblyLine->code) {
  case MUL:
    return setRegistersMultiply(instruction, code);
  case MLA:
  /* Check whether the input is valid 
     and the elements of the code are registers */
    if(code[4][0] != 'r') {
      return INVALID_INPUT;
    }
    *instruction = setBits(1, 21, *instruction);
    int rn = atoi(strtok(code[4], "r"));
    *instruction = setBits(rn, 12, *instruction);
    return setRegistersMultiply(instruction, code); 
  default:
    return INVALID_INPUT;
  }
}

/* Take an expression with or without [] brackets, return the number of arguments in brackets;
   assumes destTok[MAX_EXPR_IN_BRACKETS][MAX_INSTRUCTION_SIZE] */
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

/* In the case of ldr Rd,=<expr> type dataTransfer instructions */
static int dataTransferImmediate(ldrAddresses *ldrAddresses, char **code, Instruction *instruction) {
  Address expression = (Address) strtol(strtok(code[2], "="), NULL, 0);

  if (expression <= 0xFF) {
    /* Treat as a mov instruction, with the condition code - al */
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

  /* Set 0xF in place of Rn */
  *instruction = setBits(0xF, 16, *instruction);

  return err;
}

/* In the case of <ldr/str> Rd,<address> (where <address> is 1-3 tokens) */
static int setDataTransfer(Instruction *instruction, char **code,
			   ldrAddresses *ldrAddresses, symbolNode *operationNode) {
  *instruction = 0;
  
  /* Set condition code and instruction identity bit */
  *instruction = setBits(al, 28, *instruction);
  *instruction = setBits(1, 26, *instruction);

  /* Set Rd register */
  int rd = strtol(strtok(code[1], "r"), NULL, 0);
  *instruction = setBits(rd, 12, *instruction);

  /* default: U bit is set (to add offset) */
  *instruction = setBits(1, 23, *instruction);

  switch(operationNode->data.assemblyLine->code) {
  case LDR:
    /* code[0] is "ldr" */
    /* set the L bit */
    *instruction = setBits(1, 20, *instruction);

    if(code[2][0] == '=') {
      /* code[2] is an immediate value (only possible for ldr) */
      /* set the P bit */
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

  /* code[0] is "ldr" or "str", 
     contains split code[2], initialised to NULL
     or contains code[2-4] */
  char **arg2 = calloc(MAX_EXPR_IN_BRACKETS * MAX_INSTRUCTION_SIZE, sizeof(char));
  int arguments = removeBrackets(arg2, code[2]);

  int err = OK;

  if(!code[3]) {
    /* Pre-indexing: set P bit */
    *instruction = setBits(1, 24, *instruction);

  } else if (arguments == 1) {
    // Post-indexing

    int i = 3;
    while (code[i]) {
      arg2[i - 2] = code [i];
      i++;
    }

    arguments = i - 2;
    
  } else { 
    return INVALID_INSTRUCTION;
  }

  switch (arguments) {
      case 0:
	printf("Invalid dataTransfer formatting");
	return INVALID_INSTRUCTION;
      case 1:
	/* arg2[0] is register address (Rn),
	   offset is 0 so nothing needs to be set */
	break;	
      case 2:
        if(arg2[1][0] == 'r') {
	  /* Optional:
	       arg2[0] is address register (Rn)
	       arg2[1] is offset register (Rm)
	       shift is 0, type is NULL (arg2[2] is NULL) */
          if(arg2[1][0] == '-') {
	    /* Clear U bit if negative */
	    *instruction &= ~(1 << 23);
	  }

	  /* Set the I bit */
	  *instruction = setBits(1, 25, *instruction);

	  /* Set rm register */
	  int rm = strtol(strtok(arg2[1], "-+r"), NULL, 0);
	  *instruction = setBits(rm, 0, *instruction);
	  break;
	} 

      /* all other cases pass to case 4 */
      case 4:
	/* Optional:
	     arg2[0] is register address (Rn)
	     arg2[1] is offset register with sign ({+/-}Rm) (unset U bit (23) if negative)
	     arg2[2] is a shift expression (eg. lsr #2) */
	if(arg2[1][0] == '-' || arg2[1][1] == '-') {
	  /* Clear U bit if negative */
	  *instruction &= ~(1 << 23);
	}
	
	err = calculateOperand(instruction, arg2, 1, 0);
	break;
      default:
	printf("Invalid dataTransfer formatting");
	return INVALID_INSTRUCTION;
    }

  /* Set rn register */
  int rn = strtol(strtok(arg2[0], "r"), NULL, 0);
  *instruction = setBits(rn, 16, *instruction);
  
  free(arg2);
  return err;
}

static int setBranch(Instruction *instruction, char **code,
		     ldrAddresses *ldrAddresses, symbolNode *operationNode, symbolNode *symbolTable) {

  *instruction = 0;

  /* Set the 101 at bits 27 - 25 and the condition code */
  *instruction = setBits(5, 25, *instruction);
  *instruction = setBits(operationNode->data.assemblyLine->conditionCode, 28, *instruction);

  symbolNode *assemblyOffset = search(symbolTable, code[1]);
  
  int32_t offset;
  if(!assemblyOffset) {
    /* The address is a constant - set the address to it */
    offset = strtol(code[1], NULL, 0);
    
  } else {
    /* The address is a label - find the corresponding address */
     Address currentAddress = *(ldrAddresses->currAddress);
     offset = (assemblyOffset->data.address) - (currentAddress + 8);
  }
  
  /* The offset is more than 26 bits */
  if(offset > 0x1FFFFFF || offset < -0x1FFFFFF) {
    return INVALID_INSTRUCTION;
  }
  
  offset >>= 2;
  offset = offset & ((1 << 24) - 1);
  *instruction = setBits(offset, 0, *instruction);

  return OK;
}

static int setHalt(Instruction *instruction) {
  *instruction = 0;
	return 0;
}

static int setSpecialInstruction(Instruction *instruction, char **code, symbolNode *operationNode) {
  int err;
  char **newCode = calloc(MAX_INSTRUCTION_PARAMS, sizeof(char*));
 
  newCode[0] = strdup("mov");
  newCode[4] = code[2];
  newCode[2] = code[1];
  newCode[1] = code[1];
  newCode[3] = code[0];
  err = setDataProcessing(instruction, newCode, operationNode);


  free(newCode[0]);
  free(newCode);
  return err;
}

int assemble(Instruction *setInstruction, symbolNode *symbolTable,
	     char **nextInstruction, ldrAddresses *ldrAddresses) {
  /* check if nextInstruction is a label */
  if(!nextInstruction[1]) {
    return NOT_INSTRUCTION;
  }

  symbolNode *operationNode = search(symbolTable, nextInstruction[0]);

  if (!operationNode) {
    return NOT_IN_TABLE;
  }
  
  InstructionType type = operationNode->data.assemblyLine->type;

  switch(type) {
  case BRANCH:
    return setBranch(setInstruction, nextInstruction, ldrAddresses, operationNode, symbolTable);
  case DATA_PROCESSING:
    return setDataProcessing(setInstruction, nextInstruction, operationNode);
  case DATA_TRANSFER:
    return setDataTransfer(setInstruction, nextInstruction, ldrAddresses, operationNode);
  case MULTIPLY:
    return setMultiply(setInstruction, nextInstruction, operationNode);
  case HALT:
    return setHalt(setInstruction);
  case SHIFT:
    return setSpecialInstruction(setInstruction, nextInstruction, operationNode);
  default:
    return INVALID_INSTRUCTION;
  }
}

char *getProgramError(errorCode e) {
  errorType errors[SYS + 1];
  errors[INVALID_INSTRUCTION].code = INVALID_INSTRUCTION;
  errors[INVALID_INSTRUCTION].message = "Invalid instruction";
  
  errors[INVALID_SHIFT].code = INVALID_SHIFT;
  errors[INVALID_SHIFT].message = "Invalid shift";
  
  errors[INVALID_INPUT].code = INVALID_INPUT;
  errors[INVALID_INPUT].message = "Invalid input";

  errors[NOT_INSTRUCTION].code = NOT_INSTRUCTION;
  errors[NOT_INSTRUCTION].message = "Is a label, not an instruction";

  errors[NOT_IN_TABLE].code = NOT_IN_TABLE;
  errors[NOT_IN_TABLE].message = "Label/pneumonic was not found in the symbol table";

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

