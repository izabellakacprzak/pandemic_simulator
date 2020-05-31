#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include "pipeline_utils.h"
#include "execute_utils.h"

#define MASK_16 (1 << 16) - 1

// loads the instructions read from a file into memory
int loadToMemory(State *currentStatePtr, char *filepath){
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

    fread(&currentStatePtr->memory[i + 3], 1, 1, sourceFile);
    fread(&currentStatePtr->memory[i + 2], 1, 1, sourceFile);
    fread(&currentStatePtr->memory[i + 1], 1, 1, sourceFile);
    fread(&currentStatePtr->memory[i], 1, 1, sourceFile);
    i += 4;
  }

  return fclose(sourceFile);
}


int main(int argc, char **argv) {
  assert (argc == 2);

  // a structure representing the current state including memory and registers
  State currentState = { 0 };
  State *currentStatePtr = &currentState;

  // a structure representing the instructions
  // currently being fetched, decoded and executed
  Pipeline currentPipeline = { 0 };
  Pipeline *currentPipelinePtr = &currentPipeline;

  InstructionType decodedInstruction;

  // loading all the instructions from the given file into memory
  loadToMemory(currentStatePtr, argv[1]);

  // fetching the first instruction
  fetchInstruction(currentStatePtr, currentPipelinePtr);
	
  // fetching the second instruction
  //fetchInstruction(currentStatePtr, currentPipelinePtr);

  // keeping track of the PC
  Register currentPC;
	
  do {

    // fetching a new instruction
    // passing the decoded into executed
    fetchInstruction(currentStatePtr, currentPipelinePtr);

    // keeping what the PC was before execution
    currentPC = currentState.regPC;
    
    // decoding the instruction from currentPipeline.decoded
    decodedInstruction = determineType(currentPipeline.decoded);
		
    // checking if the previously decoded instruction
    // is valid (checcing the condition code)
    // and if so executing
    if(determineValidity(currentPipeline.decoded, currentStatePtr)){
      execute(currentPipeline.decoded, currentStatePtr, decodedInstruction);

      // if there is a branch (regPC has changed) reset the pipeline
      if (currentPC != currentState.regPC) {
	fetchInstruction(currentStatePtr, currentPipelinePtr);
      }
    }
		
  } while (decodedInstruction != HALT);

  terminate(currentStatePtr);

  return EXIT_SUCCESS;
}
