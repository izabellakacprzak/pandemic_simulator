#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "combinedlib/combined_utils.h"
#include "assemblelib/assemble_utils.h"
#include "assemblelib/text_utils.h"
#include "assemblelib/tree.h"

Instruction *makeExtraInstructionsArray(Address size) {
  Instruction *out = calloc(size, sizeof(Instruction));
  return out;
}

/*
  argv[1] -> input file
  argv[2] -> output file
*/
/* Implementation of a two-pass assembler */
int main(int argc, char **argv) {
  assert (argc == 3);

  errorCode currentStatus = OK;
  char *source = argv[1], *dest = argv[2];
  FILE *sourceFile, *destFile = NULL;
  Address currAddress = 0;
  symbolNode *symbolTable = NULL; //creates an empty symbol table
  char **currentLine = calloc((MAX_INSTRUCTION_PARAMS + 1), sizeof(char*));
  FATAL_SYS((currentLine == NULL));
  /*
  for(int i = 0; i < (MAX_INSTRUCTION_PARAMS + 1); i++){
    currentLine[i] = calloc((MAX_INSTRUCTION_SIZE + 1), sizeof(char));
    FATAL_SYS((currentLine[i] == NULL));
  }
  */

  ldrAddresses loadConstants = {0};


  /* First pass -> creating the symbol table */
  sourceFile = fopen(source, "r");
  FATAL_SYS((sourceFile == NULL)); //file loading failed
  currentStatus = loadNextInstruction(currentLine, sourceFile);
  
  FATAL_PROG((currentStatus != OK && currentStatus != END_OF_FILE), currentStatus);
  int label;
  treeData data;

  printf("Opened source file\n");


  while(currentStatus != END_OF_FILE) {
    

	  printf("%s\n", *currentLine);
      	  /* Check if the first token is a label */
    label = isLabel(currentLine);
    if(label == 1) {
	    printf("3\n");
      	    data.address = currAddress; 
    } else if(label == -1) {
	    printf("4\n");
      /* An invalid instruction is detected */
      FATAL_PROG(1,INVALID_INSTRUCTION);
    } else {
      data.assemblyLine = getDataFromOperation(currentLine[0]);
      printf("hello\n");
      FATAL_PROG(data.assemblyLine == NULL, INVALID_INSTRUCTION);
    }

    printf("hej\n");

    //adds data to symbol table
    symbolTable = insert(symbolTable, currentLine[0], data, label);
    //insert return the root of the tree
    FATAL_PROG(symbolTable == NULL, OUT_OF_MEMORY); //only of node allocation fails
    
    /* Load next instruction and increment address by 4 */
    currentStatus = loadNextInstruction(currentLine, sourceFile);
    FATAL_PROG((currentStatus != OK && currentStatus != END_OF_FILE), currentStatus);
    if (!label) {
      currAddress = currAddress + 4;
    }
  }


  printf("Initialized label tree\n");


  currentStatus = OK;
  FATAL_SYS((fclose(sourceFile) != 0));

  /* Second pass -> generate encoded instructions */
  loadConstants.lastAddress = currAddress; //largest address
  loadConstants.currAddress = &currAddress;

  /*max number of elements is number of instructions assembled 
    if every instruction is an ldr with immediate value */

  Instruction *extraInstructions = makeExtraInstructionsArray(currAddress);
  FATAL_PROG(extraInstructions == NULL, OUT_OF_MEMORY);
  
  loadConstants.extraInstructions = extraInstructions;

  currAddress = 0;
  /* Reopen source file to go to the top */
  sourceFile = fopen(source, "r");
  FATAL_SYS((sourceFile == NULL));
  destFile = fopen(dest, "w");
  FATAL_SYS((destFile == NULL));

  currentStatus = loadNextInstruction(currentLine, sourceFile);
  FATAL_PROG((currentStatus != OK && currentStatus != END_OF_FILE), currentStatus);
  Instruction result;
  

  printf("About to start assemble\n");

  while(currentStatus != END_OF_FILE) {
    result = 0;
    currentStatus = assemble(&result, symbolTable, currentLine, &loadConstants);

    if (currentStatus != NOT_INSTRUCTION) {
      currAddress += 4;

      //detects if something goes wrong assembling an instruction
      FATAL_PROG(currentStatus != OK, currentStatus);
    
      currentStatus = writeNextInstruction(result, destFile);
      FATAL_PROG(currentStatus != OK, currentStatus);
    }
    
    currentStatus = loadNextInstruction(currentLine, sourceFile);
    FATAL_PROG((currentStatus != OK && currentStatus != END_OF_FILE), currentStatus);
  }

  //Adds offsets for immediate value ldrs to the end of the assembly code
  for (int i = 0; i < loadConstants.length; i++) {
    currentStatus = writeNextInstruction(loadConstants.extraInstructions[i], destFile);
    FATAL_PROG(currentStatus != OK, currentStatus);
  }

  FATAL_SYS((fclose(sourceFile) != 0));
  FATAL_SYS((fclose(destFile) != 0));

 fatalError: //ends the program immediately
  /* Free any dynamically alocated memory */

  /*
  if (currentLine != NULL) {
    for(int i = 0; i < (MAX_INSTRUCTION_PARAMS + 1); i++){
      free(currentLine[i]);
    }
  }
  */

  printf("free1\n");
  free(currentLine);
  printf("free2\n");
  freeTable(symbolTable);
  printf("free3\n");
  free(extraInstructions);

  if (currentStatus == OK || currentStatus == END_OF_FILE) {
	  return EXIT_SUCCESS;
  }
  
  /* Print an error message*/
  char *errorMessage;
  if (EC_IS_SYS_ERROR(currentStatus)) {
    errorMessage = strerror(EC_TO_SYS_ERROR(currentStatus));
  } else {
    errorMessage = getProgramError(currentStatus);
  }
  printf("%s on line %d\n", errorMessage, currAddress / 4);
  return EXIT_FAILURE;

}
