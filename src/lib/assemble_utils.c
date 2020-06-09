#include <stdint.h>
#include <string.h>
#include "combined_utils.h"
#include "assemble_utils.h"

const char *DATA_PROCESSING[15] = {"and", "eor", "sub", "rsb", "add", "orr",
	"mov", "tst", "teq", "cmp", "lsl"};
const char *MULTIPLY[5] = {"mul", "mla"};
const char *DATA_TRANSFER[5] = {"str", "ldr"};
const char *BRANCH[5] = {"b"};
				

// takes current instruction and sets a register at bit,
//   or just takes bit and can be used as a mask
static Instruction setRegister(int regNum, int bit) {
	return 0;
}

// all of these probably take char *nextInstruction or nothing at all
static Instruction setDataProcessing(const char *nextInstruction) {
	return 0;
}

static Instruction setMultiply() {
	return 0;
}

static Instruction setDataTransfer() {
	return 0;
}

static Instruction setBranch() {
	return 0;
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
}
