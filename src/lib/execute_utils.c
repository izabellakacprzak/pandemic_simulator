#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

#include "execute_utils.h"
#include "pipeline_utils.h"

//creates a mask of 1s from start to end
#define CREATE_MASK(start, end) ((1 << (start + 1)) - (1 << end))

#define OPERATOR_NONSHIFT(name, operator)				\
  static Register name(const Register operand1, const Register operand2) \
  {									\
    return operand1 operator operand2;					\
  }

#define OPERATOR_SHIFT(name, operator)					\
  static Register name(const Register value, const uint32_t shift)	\
  {									\
    return value operator shift;					\
  }

#define GET_FLAG(flag, instruction) (instruction & (1 << flag))

typedef int (*execution_function)(Instruction, State*);
typedef Register (*shift_function)(Register, uint32_t);

// defining operator functions for use in execute_data_processing
//   arithmetic
OPERATOR_NONSHIFT(and, &)
OPERATOR_NONSHIFT(eor, ^)
OPERATOR_NONSHIFT(or, |)
OPERATOR_NONSHIFT(mov, *0+)

  static Register add(int isCarry, State *statePtr, Register operand1, Register operand2) {
  if (isCarry) {
    setC(statePtr, ((operand2 > 0) && (operand1 > INT_MAX - operand2 ))
	 || ((operand2 < 0) && (operand1 < INT_MIN - operand2)));
  } //sets C if there is an overflow or underflow and S is set
  return operand1 + operand2;
}

static Register sub(int isCarry, State *statePtr, Register operand1, Register operand2) {
  if (isCarry) {
    setC(statePtr, ((operand2 < 0) && (operand1 > INT_MAX + operand2 ))
	 || ((operand2 > 0) && (operand1 < INT_MIN + operand2)));
  } //sets C if there is an overflow or underflow and S is set
  return operand1 - operand2;
}

static Register rsb(int isCarry, State *statePtr, Register operand1, Register operand2) {
  return sub(isCarry, statePtr, operand2, operand1);
}

//  shifts
OPERATOR_SHIFT(lsl, <<)
OPERATOR_SHIFT(lsr, >>)

  static Register asr(Register value, uint32_t shift){
  if (shift > 32) {
    shift = 32;
  }

  int sign = value & (1 << 31);
  value = value >> shift;

  if (sign) {
    return value | ~((1 << (32 - shift)) - 1); // CREATE_MASK(31, (32 - shift));
  }

  return value;
}

static Register ror(Register value, uint32_t shift){
  Register shifted, rotated;
  shift %= 32;

  shifted = value >> shift;
  rotated = value << (32 - shift);
  return shifted | rotated;
}

// start of execute functions

static int execute_halt(Instruction instruction, State *statePtr){
  return 0;
}

// if you want to change the carry bit then carry = 1
// loads register offset given an instruction and state by checking bits 11-0
static Register get_offset_register(int carry, Instruction instruction, State *statePtr){
  Register value = *getRegPointer(3, statePtr, instruction);

  // check bits 6-5 to find what shift to use on register value
  shift_function shifts[4] = {lsl, lsr, asr, ror};
  shift_function selectedShift = shifts[(instruction & CREATE_MASK(6, 5)) >> 5];

  // amount could be a Register or an immediate
  Register amount;
  
  // Check 4th bit
  if (instruction & (1 << 4)) {
    // 4th bit is 1  -> amount is a Register
    amount = *getRegPointer(11, statePtr, instruction);
  } else {
    // 4th bit is 0  -> amount is an immediate
    amount = (instruction & CREATE_MASK(11, 7)) >> 7;
  }

  if (carry) {
    int carryBit;
    
    if (selectedShift == lsl) {
      carryBit = (instruction & (1 << (32 - amount))) >> (32 - amount);
    } else {
      carryBit = (instruction & (1 << (amount - 1))) >> (amount - 1);
    }

    setC(statePtr, carryBit);
  }

  return selectedShift(value, amount);
}

static Register callOperator(int sFlag, State *statePtr, const Operator *operator,
			     Register operand1, Register operand2) {
  if (operator->isArithmetic) {
    return operator->operation.operationWithCarry(sFlag, statePtr, operand1, operand2);
  }
  
  return operator->operation.operationWithoutCarry(operand1, operand2);
}

static int execute_data_processing(Instruction instruction, State *statePtr) {
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
    printf("ERROR: invalid operand\n");
    return 1;
  }

  // checking the I flag
  if (GET_FLAG(I,instruction)) {
    // operand2 is an immediate constant
    operand2 = instruction & CREATE_MASK(7, 0);
    // rotate  = rotate * 2
    uint32_t rotate = (instruction & CREATE_MASK(11, 8)) >> 7;

    operand2 = ror(operand2, rotate);
  } else {
    // find operand2 using a register to shift, set carry if not an arithmetic function and flags to be written
    operand2 = get_offset_register(!operator.isArithmetic && sFlag, instruction, statePtr);
  }

  Register res = callOperator(sFlag, statePtr, &operator, operand1, operand2); 
  if (operator.isWritten) {
    *destination = res;
  }

  if (sFlag) { //if S is set
    setN(statePtr, res);
    setZ(statePtr, res);
    //setC done in operator functions or operand2 loading
  }
  
  return 0;
}

static int execute_multiply(Instruction instruction, State *statePtr){
  //get destination register pointer - bits 19 to 16 in instr
  // Rm - bits 3 to 0
  // Rs - bits 11 to 8
      
  Register *regRd = getRegPointer(19, statePtr, instruction);
  Register *regRs = getRegPointer(11, statePtr, instruction);
  Register *regRm = getRegPointer(3, statePtr, instruction);
      
  // check whether to perform multiply and accumulate or
  // multiply only - function for now in pipeline_utils
  if(GET_FLAG(A, instruction)){

    // set destination register to Rm x Rs + Rn
    // Rn - bits 15 to 12

    Register *regRn = getRegPointer(15, statePtr, instruction);
    *regRd = (*regRm) * (*regRs) + (*regRn);	

  } else{

    //set destination register to Rm x Rs
    *regRd = (*regRm) * (*regRs);
	
  }

  if(GET_FLAG(S, instruction)){

    //set result to the value in destination register
    int result = *regRd;
    setZ(statePtr, result);
    setN(statePtr, result);

  }

  // return 0 if successful;
  return 0;
}

int invalidMemoryAccess(int memAddress){
  if(memAddress < 0 || memAddress > MEMORY_SIZE){
    printf("Error: Out of bounds memory access at address 0x%08x\n", memAddress);
    return 1;
  }
  return 0;
}

static int execute_data_transfer(Instruction instruction, State *statePtr) {
  int offset = instruction & ((1 << 12) - 1);

  Register *destReg = getRegPointer(15, statePtr, instruction);
  Register *baseReg = getRegPointer(19, statePtr, instruction);

  int memAddress = 0;
  int regAddress = 0;

  if(!(GET_FLAG(U, instruction))){
    // subtracting offset
    offset = -offset;
  }
  if(GET_FLAG(I, instruction)){
    offset = get_offset_register(0, instruction, statePtr);
  }

  if(GET_FLAG(P, instruction)){
    // pre-indexing	
    if(GET_FLAG(L, instruction)){
      // loading
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
      // storing
      memAddress = *baseReg + offset;
      if(invalidMemoryAccess(memAddress)
	 || invalidMemoryAccess(memAddress + 1)
	 || invalidMemoryAccess(memAddress + 2)
	 || invalidMemoryAccess(memAddress + 3))
	return 1;
      statePtr->memory[memAddress + 3] = CREATE_MASK(7, 0) & *destReg;
      statePtr->memory[memAddress + 2] = CREATE_MASK(7, 0) & (*destReg >> 8);
      statePtr->memory[memAddress + 1] = CREATE_MASK(7, 0) & (*destReg >> 16);
      statePtr->memory[memAddress] = CREATE_MASK(7, 0) & (*destReg >> 24);
    }
  } else {
    // post-indexing
    if(GET_FLAG(L, instruction)){
      // loading
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
      // storing
      memAddress = *baseReg;
      if(invalidMemoryAccess(memAddress)
	 || invalidMemoryAccess(memAddress + 1)
	 || invalidMemoryAccess(memAddress + 2)
	 || invalidMemoryAccess(memAddress + 3))
	return 1;
      statePtr->memory[memAddress + 3] = CREATE_MASK(7, 0) & *destReg;
      statePtr->memory[memAddress + 2] = CREATE_MASK(7, 0) & (*destReg >> 8);
      statePtr->memory[memAddress + 1] = CREATE_MASK(7, 0) & (*destReg >> 16);
      statePtr->memory[memAddress] = CREATE_MASK(7, 0) & (*destReg >> 24);
      *baseReg += offset;
    }
  }
  return 0;

}

static int execute_branch(Instruction instruction, State *statePtr){
  int32_t extendedOffset = (CREATE_MASK(23, 0) & instruction) << 2;
  Register oldPC = statePtr->regPC;
  
  if (extendedOffset & (1 << 25)) {
    extendedOffset = extendedOffset | ~((1 << 26) - 1);
  }

  statePtr->regPC += extendedOffset;

  if(statePtr->regPC < 0 || statePtr->regPC >= MEMORY_SIZE){
    printf("Error: Out of bounds memory access at address 0x%08x", statePtr->regPC);
    //undoes this branch as it is invalid
    statePtr->regPC = oldPC; 
    return 1;
  }
  statePtr->branchFlag = 1;
  return 0;
}

Register *getRegPointer(int reg, State *statePtr, Instruction instruction){
  // getting the 4 bits of the instruction
  // which correspond to the register which
  // starts at bit reg
  int regAddress = ((1 << 4) - 1) & (instruction >> (reg - 4 + 1));
  return &statePtr->registers[regAddress];
}

int execute(Instruction instruction, State *statePtr, InstructionType type) {
  //instruction to be executed, machine state, boolean on whether to execute, instruction type
  execution_function executions[5];

  //initialises the array with function pointers for each instruction type
  //based on the ordering of instr_type enum
  executions[0] = execute_branch;
  executions[1] = execute_data_transfer;
  executions[2] = execute_data_processing;
  executions[3] = execute_multiply;
  executions[4] = execute_halt;
  
  return (executions[type](instruction, statePtr));
  //executes the function corresponding to the type of instruction by the enum's value
}
