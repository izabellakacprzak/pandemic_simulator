#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "pipeline_utils.h"
#include "execute_utils.h"

/* Loads the instructions from a file with the specified filepath into memory */
int loadToMemory(State *currentStatePtr, char *filepath) {
  FILE *sourceFile;
  sourceFile = fopen(filepath, "rb");

  if(!sourceFile) {
    printf("Error: Can't open file");
    exit(EXIT_FAILURE);
  }

  int i = 0;

  while(!feof(sourceFile)) {
    
    if(i >= MEMORY_SIZE) {
      printf("Error: out of bounds memory access at address 0x%08x", i);
    }

    fread(&currentStatePtr->memory[i], 1, 1, sourceFile);
    fread(&currentStatePtr->memory[i + 1], 1, 1, sourceFile);
    fread(&currentStatePtr->memory[i + 2], 1, 1, sourceFile);
    fread(&currentStatePtr->memory[i + 3], 1, 1, sourceFile);
    i += 4;
  }
  
  return fclose(sourceFile);
}

/* Emulates the execution of an ARM binary file */
int main(int argc, char **argv) {
  assert (argc == 2);

  /* Initializes a structure which represents 
     the current state of the memory and the registers */
  State currentState = { 0 };
  State *currentStatePtr = &currentState;

  /* Initializes a structure which represents the instructions
     which are currently being fetched, decoded and executed */
  Pipeline currentPipeline = { 0 };
  Pipeline *currentPipelinePtr = &currentPipeline;
  InstructionType decodedInstruction;
 
  loadToMemory(currentStatePtr, argv[1]);

  /* Fetches the first instruction before the creation of the emulator loop */
  fetchInstruction(currentStatePtr, currentPipelinePtr);

  /* The emulator loop which will run until a
      decoded instruction is all-0 */
  do {
    fetchInstruction(currentStatePtr, currentPipelinePtr);
    
    decodedInstruction = determineType(currentPipeline.decoded);
		
    /* Checks the validity of the previously decoded instruction 
       and if valid - executes it */
    if(determineValidity(currentPipeline.decoded, currentStatePtr)) {
      execute(currentPipeline.decoded, currentStatePtr, decodedInstruction);

      /* Checks whether a branch instruction has been executed
         and if so - resets the pipeline */
      if(currentState.branchFlag) {
	fetchInstruction(currentStatePtr, currentPipelinePtr);
	currentState.branchFlag = 0;
      } 
    }		
  } while(decodedInstruction != HALT);

  /* Upon termination prints out the state of the registers 
     and the contents of any non-zero memory location */
  terminate(currentStatePtr);

  return EXIT_SUCCESS;
}
