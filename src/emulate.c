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

// fetches an instruction from memory at the regPC address
void fetchInstruction(struct CurrentState currentState, struct Pipeline currentPipeline){
  // Shifting the pipeline
  currentPipeline.executed = currentPipeline.decoded;
  currentPipeline.decoded = currentPipeline.fetched;
  
  // Fetching next instruction, masking the PC adress, and incramenting PC
  currentPipeline.fetched = currentState.memory[(currentState.regPC) & (MASK_16)];
  currentState.regPC += 4;
}
  
int main(int argc, char **argv) {
  assert (argc == 2);
  
  // a structure representing the current state including memory and registers
  struct CurrentState currentState = { 0 };

  // a structure representing the instructions currently being fetched, decoded and executed
  struct Pipeline currentPipeline = { 0 };
  
  loadToMemory(currentState, argv[1]);

  fetchInstruction(currentState, currentPipeline);
  
  while (1) {
    // Fetch

    fetchInstruction(currentState, currentPipeline);

    // Decode
    
    

    // Execute

    
    
    return EXIT_SUCCESS;
  }
  
  return EXIT_SUCCESS;
}
