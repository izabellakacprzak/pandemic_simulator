#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include "pipeline_utils.h"

#define MASK_16 (1 << 16) - 1

int loadToMemory(struct CurrentState *currentStatePtr, char *filepath){
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

    fread(&(*currentStatePtr).memory[i], 4, 1, sourceFile);
    i += 4;
  }
  
  return fclose(sourceFile);
}

void fetchInstruction(struct CurrentState *currentStatePtr, struct Pipeline *currentPipelinePtr){
  // Shifting the pipeline
  (*currentPipelinePtr).executed = (*currentPipelinePtr).decoded;
  (*currentPipelinePtr).decoded = (*currentPipelinePtr).fetched;
  
  // Fetch next instruction, masking the PC adress, and increment PC
  
  //(*currentPipelinePtr).fetched = (*currentStatePtr).memory[((*currentStatePtr).regPC) & (MASK_16)];
  uint8_t first = (*currentStatePtr).memory[(*currentStatePtr).regPC];
  uint8_t second = (*currentStatePtr).memory[(*currentStatePtr).regPC + 1];
  uint8_t third = (*currentStatePtr).memory[(*currentStatePtr).regPC + 2];
  uint8_t fourth = (*currentStatePtr).memory[(*currentStatePtr).regPC + 3];

    
  (*currentPipelinePtr).fetched = (first << 24) | (second << 16) | (third << 8) | fourth;
  (*currentStatePtr).regPC += 4;
}
  
int main(int argc, char **argv) {
  assert (argc == 2);
  
  struct CurrentState currentState = { 0 };
  struct Pipeline currentPipeline = { 0 };

  struct CurrentState *currentStatePtr = &currentState;
  struct Pipeline *currentPipelinePtr = &currentPipeline;
  
  loadToMemory(currentStatePtr, argv[1]);

  fetchInstruction(currentStatePtr, currentPipelinePtr);

  // do we need an extra decode?
  
  while (1) {
    // Fetch

    fetchInstruction(currentStatePtr, currentPipelinePtr);

    // Decode
    
    InstructionType type;
    type = determineType(currentPipeline.decoded);

    // Execute

    
    if (type == HALT) {
      return EXIT_SUCCESS;
    }
  }
  
  return EXIT_FAILURE;
}
