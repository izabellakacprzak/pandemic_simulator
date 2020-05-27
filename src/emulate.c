#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>

#define MEMORY_SIZE 32768
#define MASK_16 (1 << 16) - 1

/* Registers stored as 32 bit unsigned ints:
     0-12  - general use
     13-14 - SP, LR (ignored for this)
     15    - PC
     16    - CPSR
 */

typedef uint32_t instruction;

struct CurrentState{
  // general purpose registers
  uint32_t reg0;
  uint32_t reg1;
  uint32_t reg2;
  uint32_t reg3;
  uint32_t reg4;
  uint32_t reg5;
  uint32_t reg6;
  uint32_t reg7;
  uint32_t reg8;
  uint32_t reg9;
  uint32_t reg10;
  uint32_t reg11;
  uint32_t reg12;
  
  // SP register (ignored for this exercise)
  uint32_t reg13;
  
  // LR register (ignored for this exercise)
  uint32_t reg14;
  
  // PC register
  uint32_t regPC;
  
  // CPSR register
  uint32_t regCPSR;

  uint8_t memory[MEMORY_SIZE];
};

struct Pipeline{
  instruction fetched;
  instruction decoded;
  instruction executed;
};

enum InstructType {HALT, DATA_PROCESSING, MULTIPLY, SINGLE_DATA_TRANSFER, BRANCH};

int loadToMemory(struct CurrentState currentState, char *filepath){
  FILE *sourceFile;
  sourceFile = fopen(filepath, "rb")
   
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

void fetchInstruction(struct CurrentState currentState, struct Pipeline currentPipeline){
  // Shifting the pipeline
  currentPipeline.executed = currentPipeline.decoded;
  currentPipeline.decoded = currentPipeline.fetched;
  
  // Fetch next instruction, masking the PC adress, and incrament PC
  currentPipeline.fetched = currentState.memory[(currentState.regPC) & (MASK_16)];
  currentState.regPC += 4;
}
  
int main(int argc, char **argv) {
  assert (argc == 2);
  
  struct CurrentState currentState = { 0 };
  struct Pipeline currentPipeline = { 0 };
  
  loadToMemory(currentState, argv[1]);

  fetchInstruction(currentState, currentPipeline);
  
  while (1) {
    // Fetch

    fetchInstruction(currentState, currentPipeline);

    // Decode
    
    // enum InstructType type = decodeInstruction(currentState, currentPipeline);

    // Execute


    
    return EXIT_SUCCESS;
  }
  
  return EXIT_SUCCESS;
}
