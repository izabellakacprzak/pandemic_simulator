#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include "pipeline_utils.h"

#define MASK_16 (1 << 16) - 1

// loads the instructions read from a file into memory
int loadToMemory(struct CurrentState currentState, char *filepath){
	FILE *sourceFile;
	sourceFile = fopen(filepath, "rb");

	if(!sourceFile){
		perror("Error opening file");
		exit(EXIT_FAILURE);
	}

	int i = 0;

	while(!feof(sourceFile)){

		if(i >= MEMORY_SIZE){
			perror("The memory is full.");
			exit(EXIT_FAILURE);
		}

		fread(&currentState.memory[i], 4, 1, sourceFile);
		i += 4;
	}

	return fclose(sourceFile);
}

int main(int argc, char **argv) {
	assert (argc == 2);

	// a structure representing the current state including memory and registers
	struct CurrentState currentState = { 0 };

	// a structure representing the instructions
	// currently being fetched, decoded and executed
	struct Pipeline currentPipeline = { 0 };

	// loading all the instructions from the given file into memory
	loadToMemory(currentState, argv[1]);

	// fetching the first instruction
	fetchInstruction(currentState, currentPipeline);
	
	// fetching the second instruction
	fetchInstruction(currentState, currentPipeline);


	InstructionType decodedInstruction;
	while (1) {
		// decoding the instruction from currentPipeline.decoded
		decodedInstruction = determineType(currentPipeline.decoded);

		// fetching a new instruction
		// passing the decoded into executed
		fetchInstruction(currentState, currentPipeline);

		// checking if the previously decoded instruction
		// is valid (checcing the condition code)
		// and if so executing
		if(determineValidity(currentPipeline.executed, currentState)){
			//execute(decodedInstruction, currentPipeline.executed);
		}
		return EXIT_SUCCESS;
	}

	return EXIT_SUCCESS;
}
