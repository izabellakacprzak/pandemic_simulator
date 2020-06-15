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

/* Implementation of a two-pass assembler
   with  argv[1] -> input file
         argv[2] -> output file */
int main(int argc, char **argv) {
  assert (argc == 3);

  errorCode currentStatus = OK;
  char *source = argv[1], *dest = argv[2];
  FILE *sourceFile, *destFile;
  Address currAddress = 0;
  symbolNode *symbolTable = NULL;
  Instruction *extraInstructions = NULL;
  char **currentLine = calloc(MAX_INSTRUCTION_PARAMS + 1, sizeof(char*));
  
  FATAL_SYS((currentLine == NULL));
   
  ldrAddresses loadConstants = {0};

  /* First pass -> creating the symbol table */
  sourceFile = fopen(source, "r");
  /* File loading failed */
  FATAL_SYS((sourceFile == NULL));
  currentStatus = loadNextInstruction(currentLine, sourceFile);
  FATAL_PROG((currentStatus != OK && currentStatus != END_OF_FILE
		&& currentStatus != WHITESPACE_LINE), currentStatus);
  
  int label;
  treeData data;
  while(currentStatus != END_OF_FILE) {
    if(currentStatus != WHITESPACE_LINE) {
      /* Check if the first token is a label */
      label = isLabel(currentLine);
      if(label == 1) {
	data.address = currAddress; 
      } else if(label == -1) {
	/* An invalid instruction is detected */
	FATAL_PROG(1,INVALID_INSTRUCTION);
      } else {
	data.assemblyLine = getDataFromOperation(currentLine[0]);
	FATAL_PROG(data.assemblyLine == NULL, INVALID_INSTRUCTION);
      }

      /* Add mnemonic to the symbol table and check whether node allocation fails */
      symbolTable = insert(symbolTable, currentLine[0], data, label);
      FATAL_PROG(symbolTable == NULL, OUT_OF_MEMORY);
    }

    if(!label && !(currentStatus == WHITESPACE_LINE)) {
      currAddress = currAddress + 4;
    }
    
    /* Load next instruction and increment address by 4 */
    currentStatus = loadNextInstruction(currentLine, sourceFile);
    FATAL_PROG((currentStatus != OK && currentStatus != END_OF_FILE
		&& currentStatus != WHITESPACE_LINE), currentStatus);
  }

  currentStatus = OK;
  FATAL_SYS((fclose(sourceFile) != 0));

  /* Second pass -> generate encoded instructions */
  loadConstants.lastAddress = currAddress - 4; 
  loadConstants.currAddress = &currAddress;

  /* Max number of extra instructions is the worst case scenario where 
     every instruction being assembled is an ldr with immediate value */
  extraInstructions = makeExtraInstructionsArray(currAddress);
  FATAL_PROG(extraInstructions == NULL, OUT_OF_MEMORY);
  
  loadConstants.extraInstructions = extraInstructions;

  currAddress = 0;
  /* Reopen source file to go to the top */
  sourceFile = fopen(source, "r");
  FATAL_SYS((sourceFile == NULL));
  destFile = fopen(dest, "w");
  FATAL_SYS((destFile == NULL));

  currentStatus = loadNextInstruction(currentLine, sourceFile);
  FATAL_PROG((currentStatus != OK && currentStatus != END_OF_FILE
	      && currentStatus != WHITESPACE_LINE), currentStatus);
  Instruction result;
  
  while(currentStatus != END_OF_FILE) {
    result = 0;
    
    if(currentStatus != WHITESPACE_LINE) {
      currentStatus = assemble(&result, symbolTable, currentLine, &loadConstants);

      if(currentStatus != NOT_INSTRUCTION) {
	currAddress += 4;

	/* Check whether there was an error in the assembling of the instruction */
	FATAL_PROG(currentStatus != OK, currentStatus);
    
	currentStatus = writeNextInstruction(result, destFile);
	FATAL_PROG(currentStatus != OK, currentStatus);
      }
    }
    
    currentStatus = loadNextInstruction(currentLine, sourceFile);
    FATAL_PROG((currentStatus != OK && currentStatus != END_OF_FILE
		&& currentStatus != WHITESPACE_LINE), currentStatus);
  }

  /* Add offsets for immediate value ldrs to the end of the assembly code */
  for(int i = 0; i < loadConstants.length; i++) {
    currentStatus = writeNextInstruction(loadConstants.extraInstructions[i], destFile);
    FATAL_PROG(currentStatus != OK, currentStatus);
  }

  FATAL_SYS((fclose(sourceFile) != 0));
  FATAL_SYS((fclose(destFile) != 0));

  /* End the program immediately if an error has been detected */
  fatalError:
  /* Free any dynamically allocated memory */
  for (int i = 0; i < MAX_INSTRUCTION_PARAMS; i++) {
    if (currentLine[i]) {
      free(currentLine[i]);
    }
  }
  free(currentLine);
  freeTable(symbolTable);
  free(extraInstructions);

  if(currentStatus == OK || currentStatus == END_OF_FILE) {
    return EXIT_SUCCESS;
  }
  
  /* Print an error message*/
  char *errorMessage;
  if(EC_IS_SYS_ERROR(currentStatus)) {
    errorMessage = strerror(EC_TO_SYS_ERROR(currentStatus));
  } else {
    errorMessage = getProgramError(currentStatus);
  }
  printf("%s on line %d\n", errorMessage, currAddress / 4);
  return EXIT_FAILURE;

}
