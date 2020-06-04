#include <stdint.h>
#include <stdio.h>
#include "execute_utils.h"
#include "pipeline_utils.h"

/* Creates a mask of 1s from start to end */
#define CREATE_MASK(start, end) ((1 << (start + 1)) - (1 << end))

/* Creates a function out of an inbuilt arithmetic operator */
#define OPERATOR_NONSHIFT(name, operator)				   \
  static Register name(const Register operand1, const Register operand2) { \
    return operand1 operator operand2;				           \
  }

/* Creates a function out of a shift operator */
#define OPERATOR_SHIFT(name, operator)				     \
  static Register name(const uint32_t value, const uint32_t shift) { \
    return value operator shift;				     \
  }

/* Gets the flag in a given instruction */
#define GET_FLAG(flag, instruction) (instruction & (1 << flag))

typedef int (*execution_function)(Instruction, State*);
typedef Register (*shift_function)(uint32_t, uint32_t);

/* Definitions of the operator functions used in execute_data_processing */
/* Arithmetic */
OPERATOR_NONSHIFT(and, &)
OPERATOR_NONSHIFT(eor, ^)
OPERATOR_NONSHIFT(or, |)
OPERATOR_NONSHIFT(mov, *0+)
/* Shifts */  
OPERATOR_SHIFT(lsl, <<)
OPERATOR_SHIFT(lsr, >>)
  
/* Performs addition; sets the C flag if there is an overflow 
   and the S flag is set */
static Register add(int sFlag, State *statePtr, Register operand1, Register operand2) {
  Register result = operand1 + operand2;

  if(sFlag) {
    if(operand1 > 0 && operand2 > 0) {
      setC(statePtr, result < 0);
    } else if(operand1 < 0 && operand2 < 0) {
      setC(statePtr, result > 0);
    } else {
      setC(statePtr, 0);
    }    
  }
  return result;
}

/* Performs subtraction; sets the C flag if there is underflow 
   and the S flag is set */
static Register sub(int sFlag, State *statePtr, Register operand1, Register operand2) {
  Register result = operand1 - operand2;

  if(sFlag) {
    if(operand1 < 0 && operand2 > 0) {
      setC(statePtr, !(result < 0));
    } else if(operand1 > 0 && operand2 < 0) {
      setC(statePtr, !(result > 0));
    } else {
      setC(statePtr, !(operand1 < operand2));
    }
  } 
  return result;
}

/* Performs subtraction but switches operand1 and operand2
   by calling sub */
static Register rsb(int sFlag, State *statePtr, Register operand1, Register operand2) {
  return sub(sFlag, statePtr, operand2, operand1);
}

/* Performs arithmetic shift right to the value,
   preserves the sign bit */
static Register asr(uint32_t value, uint32_t shift) {
  if(shift > 32) {
    shift = 32;
  }

  int sign = value & (1 << 31);
  value = value >> shift;
  
  if(sign) {
    return value | ~((1 << (32 - shift)) - 1);
  }
  return value;
}

/* Performs rotate right to the value by rotating cyclically 
   with bit 0 shifting into bit 31 */
static Register ror(uint32_t value, uint32_t shift) {
  Register shifted, rotated;
  shift %= 32;

  shifted = value >> shift;
  rotated = value << (32 - shift);
  return shifted | rotated;
}

/* Calculates and returns the second operand (bits 11-0) as a shifted register;
   sets the C flag to the carry bit if the carry variable is 1 */
static Register getOffsetRegister(int carry, Instruction instruction, State *statePtr) {
  Register value = *getRegPointer(3, statePtr, instruction);

  /* Checks bits 6-5 to find what shift to use on the register value */
  shift_function shifts[4] = {lsl, lsr, asr, ror};
  shift_function selectedShift = shifts[(instruction & CREATE_MASK(6, 5)) >> 5];

  /* Amount could be either a Register or an immediate */
  Register amount;
  
  /* Checks whether the 4th bit is set: 
     if set - amount is a Register, else - immediate */
  if(instruction & (1 << 4)) {
    amount = *getRegPointer(11, statePtr, instruction);
  } else {
    amount = (instruction & CREATE_MASK(11, 7)) >> 7;
  }

  if(carry) {
    int carryBit;

    if(selectedShift == lsl) {
      carryBit = (instruction & (1 << (32 - amount))) >> (32 - amount);
    } else {
      carryBit = (instruction & (1 << (amount - 1))) >> (amount - 1);
    }
    
    setC(statePtr, carryBit);
  }
  return selectedShift(value, amount);
}

/* Checks the type of the operator and calls it */
static Register callOperator(int sFlag, State *statePtr, const Operator *operator,
			     Register operand1, Register operand2) {
  if (operator->isArithmetic) {
    return operator->operation.operationWithCarry(sFlag, statePtr, operand1, operand2);
  }
  
  return operator->operation.operationWithoutCarry(operand1, operand2);
}

/* Returns a pointer to a register 
   whose address starts at bit reg and is 4 bits long */
Register *getRegPointer(int reg, State *statePtr, Instruction instruction){
  int regAddress = ((1 << 4) - 1) & (instruction >> (reg - 4 + 1));
  return &statePtr->registers[regAddress];
}

/* Checks whether an out of bounds error has been performed */
static int invalidMemoryAccess(uint32_t memAddress){
  if(memAddress < 0 || memAddress > MEMORY_SIZE){
    printf("Error: Out of bounds memory access at address 0x%08x\n", memAddress);
    return 1;
  }
  return 0;
}


/* Execute functions for each instruction type */

static int executeHalt(Instruction instruction, State *statePtr) {
  return 0;
}

static int executeDataProcessing(Instruction instruction, State *statePtr) {
  Register operand1, operand2;
  Register *destination;
  int sFlag = GET_FLAG(S, instruction);
  Operator operator = {0};

  operand1 = *getRegPointer(19, statePtr, instruction);
  destination = getRegPointer(15, statePtr, instruction);

  switch((instruction & CREATE_MASK(24,21)) >> 21) {
  case 0: 
    operator.operation.operationWithoutCarry = and;
    operator.isWritten = 1;
    break;
  case 1:
    operator.operation.operationWithoutCarry = eor;
    operator.isWritten = 1;
    break;
  case 2:
    operator.operation.operationWithCarry = sub;
    operator.isWritten = 1;
    operator.isArithmetic = 1;
    break;
  case 3:
    operator.operation.operationWithCarry = rsb;
    operator.isWritten = 1;
    operator.isArithmetic = 1;
    break;
  case 4:
    operator.operation.operationWithCarry = add;
    operator.isWritten = 1;
    operator.isArithmetic = 1;
    break;
  case 8:
    operator.operation.operationWithoutCarry = and;
    break;
  case 9:
    operator.operation.operationWithoutCarry = eor;
    break;
  case 10:  
    operator.operation.operationWithCarry = sub;
    operator.isArithmetic = 1;
    break;
  case 12:
    operator.operation.operationWithoutCarry = or;
    operator.isWritten = 1;
    break;
  case 13:
    operator.operation.operationWithoutCarry = mov;
    operator.isWritten = 1;
    break;
  default:
    printf("ERROR: Invalid operand\n");
    return 1;
  }

  /* Checks whether the 4th bit is set: 
     if set - operand2 is an immediate constant, else - a shifted register */
  if(GET_FLAG(I,instruction)) {
    operand2 = instruction & CREATE_MASK(7, 0);
    /* rotate  = rotate * 2 */
    uint32_t rotate = (instruction & CREATE_MASK(11, 8)) >> 7;
    operand2 = ror(operand2, rotate);
  } else {
    /* Set carry if it's not an arithmetic function and the S flag is set */
    operand2 = getOffsetRegister(!operator.isArithmetic && sFlag, instruction, statePtr);
  }

  Register res = callOperator(sFlag, statePtr, &operator, operand1, operand2); 
  if(operator.isWritten) {
    *destination = res;
  }

  /* If the S flag is set - set the condition codes, 
     the C flag is set in the operator function 
     or the loading of operand2 */
  if(sFlag) {
    setN(statePtr, res);
    setZ(statePtr, res);
  }
  return 0;
}

static int executeMultiply(Instruction instruction, State *statePtr) {
  /* Calculate the pointers for the registers:
     Rd - bits 19 to 16 
     Rn - bits 15 to 12
     Rs - bits 11 to 8
     Rs - bits 3 to 0 */
      
  Register *regRd = getRegPointer(19, statePtr, instruction);
  Register *regRs = getRegPointer(11, statePtr, instruction);
  Register *regRm = getRegPointer(3, statePtr, instruction);
      
  
  /* If the A flag is set - perform multiply and accumulate
     if clear - perform multiply only */
  if(GET_FLAG(A, instruction)) {
    Register *regRn = getRegPointer(15, statePtr, instruction);
    *regRd = (*regRm) * (*regRs) + (*regRn);	
  } else {
    *regRd = (*regRm) * (*regRs);	
  }


  int result = *regRd;

  /* If the S flag is set - set the condition code */
  if(GET_FLAG(S, instruction)) {
    setZ(statePtr, result);
    setN(statePtr, result);
  }
  return 0;
}

static int executeDataTransfer(Instruction instruction, State *statePtr) {
  Register offset = instruction & ((1 << 12) - 1);

  Register *destReg = getRegPointer(15, statePtr, instruction);
  Register *baseReg = getRegPointer(19, statePtr, instruction);

  int memAddress = 0;
  int regAddress = 0;

  /* If the I flag is set - offset is interpreted as a shifted register */
  if(GET_FLAG(I, instruction)) {
    offset = getOffsetRegister(0, instruction, statePtr);
  }

  /* If the U flag is clear - the offset is 
     subtracted from the base register
     else - it's added */
  if(!(GET_FLAG(U, instruction))) {
    offset = -offset;
  }

  /* If the P flag is set - the offset is added to the base register
     before transferring the date (pre-indexing), 
     if clear - after (post-) */
  if(GET_FLAG(P, instruction)) {
    /* If the L flag is set the word is loaded from memory,
       if clear - the word is stored into memory */
    if(GET_FLAG(L, instruction)) {
      regAddress = *baseReg + offset;
      
      if(invalidMemoryAccess(regAddress)
	 || invalidMemoryAccess(regAddress + 1)
	 || invalidMemoryAccess(regAddress + 2)
	 || invalidMemoryAccess(regAddress + 3))
	return 1;
      
      *destReg = statePtr->memory[regAddress];

      *destReg += statePtr->memory[regAddress + 1] << 8;

      *destReg += statePtr->memory[regAddress + 2] << 16;

      *destReg += statePtr->memory[regAddress + 3] << 24;
    } else {
      memAddress = *baseReg + offset;
      
      if(invalidMemoryAccess(memAddress)
	 || invalidMemoryAccess(memAddress + 1)
	 || invalidMemoryAccess(memAddress + 2)
	 || invalidMemoryAccess(memAddress + 3))
	return 1;
      statePtr->memory[memAddress] = CREATE_MASK(7, 0) & *destReg;
      statePtr->memory[memAddress + 1] = CREATE_MASK(7, 0) & (*destReg >> 8);
      statePtr->memory[memAddress + 2] = CREATE_MASK(7, 0) & (*destReg >> 16);
      statePtr->memory[memAddress + 3] = CREATE_MASK(7, 0) & (*destReg >> 24);
    }
  } else {
    /* Post-indexing */
    if(GET_FLAG(L, instruction)) {
      /* Loading */
      regAddress = *baseReg;
      
      if(invalidMemoryAccess(regAddress)
	 || invalidMemoryAccess(regAddress + 1)
	 || invalidMemoryAccess(regAddress + 2)
	 || invalidMemoryAccess(regAddress + 3))
	return 1;
      
      *destReg = statePtr->memory[regAddress];

      *destReg += statePtr->memory[regAddress + 1] << 8;

      *destReg += statePtr->memory[regAddress + 2] << 16;

      *destReg += statePtr->memory[regAddress + 3] << 24;

      *baseReg += offset;
    } else {
      /* Storing */
      memAddress = *baseReg;
      
      if(invalidMemoryAccess(memAddress)
	 || invalidMemoryAccess(memAddress + 1)
	 || invalidMemoryAccess(memAddress + 2)
	 || invalidMemoryAccess(memAddress + 3))
	return 1;
      
      statePtr->memory[memAddress] = CREATE_MASK(7, 0) & *destReg;
      statePtr->memory[memAddress + 1] = CREATE_MASK(7, 0) & (*destReg >> 8);
      statePtr->memory[memAddress + 2] = CREATE_MASK(7, 0) & (*destReg >> 16);
      statePtr->memory[memAddress + 3] = CREATE_MASK(7, 0) & (*destReg >> 24);
      *baseReg += offset;
    }
  }
  return 0;
}

static int executeBranch(Instruction instruction, State *statePtr) {
  int32_t extendedOffset = (CREATE_MASK(23, 0) & instruction) << 2;
  Register oldPC = statePtr->regPC;
  
  if (extendedOffset & (1 << 25)) {
    extendedOffset = extendedOffset | ~((1 << 26) - 1);
  }

  statePtr->regPC += extendedOffset;

  /* Checks whether an out of bounds error has been performed */
  if(statePtr->regPC < 0 || statePtr->regPC >= MEMORY_SIZE) {
    printf("Error: Out of bounds memory access at address 0x%08x", statePtr->regPC);
    /* Undoes the branch as it is invalid */
    statePtr->regPC = oldPC; 
    return 1;
  }
  statePtr->branchFlag = 1;
  return 0;
}

int execute(Instruction instruction, State *statePtr, InstructionType type) {
  execution_function executions[5];

  /* Initialises an array with function pointers for each instruction type
     based on the value of their respective InstructionType enum */
  executions[0] = executeBranch;
  executions[1] = executeDataTransfer;
  executions[2] = executeDataProcessing;
  executions[3] = executeMultiply;
  executions[4] = executeHalt;
  
  return (executions[type](instruction, statePtr));
}
