#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

#include "execute_utils.h"
#include "pipeline_utils.h"

#define CREATE_MASK(start, end) ((1 << (start + 1)) - (1 << end))
//creates a mask of 1s from start to end
#define OPERATOR_NONSHIFT(name, operator)\
static Register name(const Register operand1, const Register operand2)\
  {\
    return operand1 operator operand2;\
  }

#define OPERATOR_SHIFT(name, operator)\
static Register name(const Register value, const uint32_t shift)\
  {\
    return value operator shift;\
  }

typedef int (*execution_function)(Instruction, State*);
typedef Register (*shift_function)(Register, uint32_t);

// defining operator functions for use in execute_data_processing
//   arithmetic
OPERATOR_NONSHIFT(and, &)
OPERATOR_NONSHIFT(eor, ^)
OPERATOR_NONSHIFT(or, |)
OPERATOR_NONSHIFT(mov, *0+)

static Register add(int isCarry, State *state, Register operand1, Register operand2) {
  if (isCarry) {
    setC(state, ((operand2 > 0) && (operand1 > INT_MAX - operand2 ))
	 || ((operand2 < 0) && (operand1 < INT_MIN - operand2)));
  } //sets C if there is an overflow or underflow and S is set
  return operand1 +operand2;
}

static Register sub(int isCarry, State *state, Register operand1, Register operand2) {
  if (isCarry) {
    setC(state, ((operand2 < 0) && (operand1 > INT_MAX + operand2 ))
	 || ((operand2 > 0) && (operand1 < INT_MIN + operand2)));
  } //sets C if there is an overflow or underflow and S is set
  return operand1 - operand2;
}

static Register rsb(int isCarry, State *state, Register operand1, Register operand2) {
  return sub(isCarry, state, operand2, operand1);
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
  uint32_t shifted, rotated;
  shift %= 32;

  shifted = value >> shift;
  rotated = value << (32 - shift);
  return shifted | rotated;
}

// start of execute functions
int execute_halt(Instruction instruction, State *state){
  return 4;
}

// if you want to change the carry bit then carry = 1
// loads register offset given an instruction and state by checking bits 11-0
Register get_offset_register(int carry, Instruction instruction, State *state){
  Register value = *getRegPointer(3, state, instruction);

  // check bits 6-5 to find what shift to use on register value
  shift_function shifts[4] = {lsl, lsr, asr, ror};
  shift_function selectedShift = shifts[(instruction & CREATE_MASK(6, 5)) >> 5];

  uint32_t amount;
  
  // Check 4th bit
  if (instruction & (1 << 4)) {
    // 4th bit is 1
    amount = *getRegPointer(11, state, instruction);
  } else {
    // 4th bit is 0
    amount = (instruction & CREATE_MASK(11, 7)) >> 7;
  }

  if (carry) {
    int carryBit;
    
    if (selectedShift == lsl) {
      carryBit = (instruction & (1 << (32 - amount))) >> (32 - amount);
    } else {
      carryBit = (instruction & (1 << (amount - 1))) >> (amount - 1);
    }

    setC(state, carryBit);
  }

  return selectedShift(value, amount);
}

static Register callOperator(int sFlag, State *state, const Operator *operator, Register operand1, Register operand2) {
  if (operator->isArithmetic) {
    return operator->operation.operationWithCarry(sFlag, state, operand1, operand2);
  }
  return operator->operation.operationWithoutCarry(operand1, operand2);
}

int execute_data_processing(Instruction instruction, State *state) {
  Register operand1, operand2;
  Register *destination;
  int sFlag = instruction & (1 << 20);
  Operator operator = {0};

  operand1 = *getRegPointer(19, state, instruction);
  destination = getRegPointer(15, state, instruction);
  
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
      perror("ERROR: invalid operand");
      return 1;
  }

  // checking the I flag
  if (instruction & (1 << 25)) {
    // operand2 is an immediate constant
    operand2 = instruction & CREATE_MASK(7, 0);
    // rotate  = rotate * 2
    uint32_t rotate = (instruction & CREATE_MASK(11, 8)) >> 7;

    operand2 = ror(operand2, rotate);
  } else {
    // find operand2 using a register to shift, set carry if not an arithmetic function and flags to be written
    operand2 = get_offset_register(!operator.isArithmetic && sFlag, instruction, state);
  }

  Register res = callOperator(sFlag, state, &operator, operand1, operand2); 
  if (operator.isWritten) {
    *destination = res;
  }

  if (sFlag) { //if S is set
    setN(state, res);
    setZ(state, res);
    //setC done in operator functions or operand2 loading
  }
  
  return 0;
}


Register *getRegPointer(int reg, State *currentState, Instruction instruction){
	// getting the 4 bits of the instruction
	// which correspond to the register which
	// starts at bit reg
	int regAddress = ((1 << 4) - 1) & (instruction >> (reg - 4 + 1));
	return &(currentState->registers.regArray[regAddress]);
}


int execute_multiply(Instruction instruction, State *state){
  //get destination register pointer - bits 19 to 16 in instr
  // Rm - bits 3 to 0
  // Rs - bits 11 to 8
      
  Register *regRd = getRegPointer(19, state, instruction);
  Register *regRs = getRegPointer(11, state, instruction);
  Register *regRm = getRegPointer(3, state, instruction);

      
  // check whether to perform multiply and accumulate or
  // multiply only - function for now in pipeline_utils
  if(getA(instruction)){

    // set destination register to Rm x Rs + Rn
    // Rn - bits 15 to 12

    Register *regRn = getRegPointer(15, state, instruction);

    *regRd = (*regRm) * (*regRs) + (*regRn);	

  } else{

    //set destination register to Rm x Rs
    *regRd = (*regRm) * (*regRs);
	
  }

  if(setCPSR(instruction)){

    //set result to the value in destination register
    int result = *regRd;
    setZ(state, result);
    setN(state, result);

  }

  // return 1 if successful;
  return 1;
}

int execute_data_transfer(Instruction instruction, State *state) {
  int offset = instruction & ((1 << 12) - 1);
  // here the function which calculates the offset
  // depending on the I bit of the instruction

  Register *destReg = getRegPointer(15, state, instruction);
  Register *baseReg = getRegPointer(19, state, instruction);

  int memAddress = 0;
  int regAddress = 0;

  if(!getU(instruction)){
    // subtracting offset
    offset = -offset;
  }
  if(getI(instruction)){
    offset = get_offset_register(0, instruction, state);
  }

  if(getP(instruction)){
    // pre-indexing	
    if(getL(instruction)){
      // loading
      regAddress = *baseReg + offset;
      *destReg = state->memory[3 - (regAddress % 4) + (regAddress / 4) * 4];
      *destReg += state->memory[3 - ((regAddress + 1) % 4) + ((regAddress + 1) / 4) * 4] << 8;
      *destReg += state->memory[3 - ((regAddress + 2) % 4) + ((regAddress + 2) / 4) * 4] << 16;
      *destReg += state->memory[3 - ((regAddress + 3) % 4) + ((regAddress + 3) / 4) * 4] << 24;
    } else {
      // storing
      memAddress = *baseReg + offset;
      state->memory[memAddress] = CREATE_MASK(7, 0) & *destReg;
      state->memory[memAddress + 1] = CREATE_MASK(7, 0) & (*destReg >> 8);
      state->memory[memAddress + 2] = CREATE_MASK(7, 0) & (*destReg >> 16);
      state->memory[memAddress + 3] = CREATE_MASK(7, 0) & (*destReg >> 24);
    }
  } else {
    // post-indexing
    if(getL(instruction)){
      // loading
      regAddress = *baseReg;
      *destReg = state->memory[3 - (regAddress % 4) + (regAddress / 4) * 4];
      *destReg += state->memory[3 - ((regAddress + 1) % 4) + ((regAddress + 1) / 4) * 4] << 8;
      *destReg += state->memory[3 - ((regAddress + 2) % 4) + ((regAddress + 2) / 4) * 4] << 16;
      *destReg += state->memory[3 - ((regAddress + 3) % 4) + ((regAddress + 3) / 4) * 4] << 24;
      *baseReg += offset;
    } else {
      // storing
      memAddress = *baseReg;
      state->memory[memAddress] = CREATE_MASK(7, 0) & *destReg;
      state->memory[memAddress + 1] = CREATE_MASK(7, 0) & (*destReg >> 8);
      state->memory[memAddress + 2] = CREATE_MASK(7, 0) & (*destReg >> 16);
      state->memory[memAddress + 3] = CREATE_MASK(7, 0) & (*destReg >> 24);
      *baseReg += offset;
    }
  }
  return 0;

}

int execute_branch(Instruction instruction, State *state){
  int32_t extendedOffset = (CREATE_MASK(23, 0) & instruction) << 2;

  if (extendedOffset & (1 << 25)) {
    extendedOffset = extendedOffset | ~((1 << 26) - 1);
  }
  
  state->registers.regStruct.regPC += extendedOffset;
  state->branchFlag = 1;
  return 0;
}

int execute(Instruction instruction, State *state, InstructionType type) {
  //instruction to be executed, machine state, boolean on whether to execute, instruction type
  execution_function executions[5];

  //initialises the array with function pointers for each instruction type
  //based on the ordering of instr_type enum
  executions[0] = execute_branch;
  executions[1] = execute_data_transfer;
  executions[2] = execute_data_processing;
  executions[3] = execute_multiply;
  executions[4] = execute_halt;
  
  return (executions[type](instruction, state));
  //executes the function corresponding to the type of instruction by the enum's value
}
