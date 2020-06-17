#include "bit_setting_helpers.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Instruction setBits(uint32_t value, int bit, Instruction instr) {
  return instr |= (value << bit);;
}

int calculateOperand(Instruction *instruction, char **code, int opIndex, int processing) {
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

int withResults(Instruction *instruction, char **code) {
	int rd = atoi(strtok(code[1], "r"));
	int rn = atoi(strtok(code[2], "r"));

	*instruction = setBits(rd, 12, *instruction);
	*instruction = setBits(rn , 16, *instruction);
	
	/* Set the operand field
	   passing 3 as the operand as the code starts at string at index 3 */
	return calculateOperand(instruction, code, 3, 1);
}

int operandAssignment(Instruction *instruction, char **code) {
	if(code[1][0] != 'r') {
		return INVALID_INPUT;
	}
       	int rd = atoi(strtok(code[1], "r"));
      	*instruction = setBits(rd, 12, *instruction);
	
	/* Set the operand field
	   passing 2 as the operand as the code starts at string at index 2 */
	return calculateOperand(instruction, code, 2, 1);
}

int noResults(Instruction *instruction, char **code) {
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

int setRegistersMultiply(Instruction *instruction, char **code) {
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

/* assumes destTok[MAX_EXPR_IN_BRACKETS][MAX_INSTRUCTION_SIZE] */
int removeBrackets(char **destTok, char *expression) {
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

int dataTransferImmediate(ldrAddresses *ldrAddresses, char **code, Instruction *instruction) {
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
