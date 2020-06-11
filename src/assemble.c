#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "combinedlib/combined_utils.h"
#include "assemblelib/assemble_utils.h"
#include "assemblelib/text_utils.h"

/*
  argv[1] -> input file
  argv[2] -> output file
*/
/* Implementation of a two-pass assembler */
int main(int argc, char **argv) {
  assert (argc == 3);

  errorCode currentStatus = OK;
  char *source = argv[1], *dest = argv[2];
  FILE *sourceFile, *destFile;
  symbolNode *symbolTable = NULL;
  char **currentLine = calloc((MAX_INSTRUCTION_PARAMS + 1), sizeof(char*));
  FATAL_SYS((currentLine == NULL));
  for(int i = 0; i < (MAX_INSTRUCTION_PARAMS + 1); i++){
    currentLine[i] = calloc((MAX_INSTRUCTION_SIZE + 1), sizeof(char));
    FATAL_SYS((currentLine[i] == NULL));
  }
   Address currAddress = 0;

  /* First pass -> creating the symbol table */
  sourceFile = fopen(source, "r");
  FATAL_SYS((sourceFile == NULL)); //file loading failed
  currentStatus = loadNextInstruction(currentLine, sourceFile);
  FATAL_PROG((currentStatus != OK && currentStatus != END_OF_FILE), currentStatus);
  int label;
  
  /* Create an empty symbol table */
  
  while(currentStatus != END_OF_FILE) {
    /* Check if the first token is a label */
    label = isLabel(currentLine);
    if(label == 1) {
      symbolTable = insert(symbolTable, currentLine[0], currAddress);
    } else if(label == -1) {
      /* An invalid instruction is detected */
      FATAL_PROG(1,INVALID_INSTRUCTION);
    }
    /* Load next instruction and increment address by 4 */
    currentStatus = loadNextInstruction(currentLine, sourceFile);
    FATAL_PROG((currentStatus != OK && currentStatus != END_OF_FILE), currentStatus);
    currAddress = currAddress + 4;
  }

  currentStatus = OK;
  FATAL_SYS((fclose(sourceFile) != 0));

  /* Second pass -> generate encoded instructions */
  Address maxAddress = currAddress;
  currAddress = 0;

  /* Reopen source file to go to the top */
  sourceFile = fopen(source, "r");
  FATAL_SYS((sourceFile == NULL));
  destFile = fopen(dest, "w");
  FATAL_SYS((destFile == NULL));

  currentStatus = loadNextInstruction(currentLine, sourceFile);
  FATAL_PROG((currentStatus != OK && currentStatus != END_OF_FILE), currentStatus);
  Instruction result = 0;
  while(currentStatus != END_OF_FILE) {
    result = assemble(symbolTable, currentLine);
    writeNextInstruction(result, destFile);
    currentStatus = loadNextInstruction(currentLine, sourceFile);
    FATAL_PROG((currentStatus != OK && currentStatus != END_OF_FILE), currentStatus);
  }

  FATAL_SYS((fclose(sourceFile) != 0));
  FATAL_SYS((fclose(destFile) != 0));
  return EXIT_SUCCESS;

 fatalError:
  /* Free any dynamically alocated memory */
  for(int i = 0; i < (MAX_INSTRUCTION_PARAMS + 1); i++){
    free(currentLine[i]);
  }
  free(currentLine);
  freeTable(symbolTable);
  /* Print an error message*/

  char *errorMessage;
  if (EC_IS_SYS_ERROR(currentStatus)) {
    errorMessage = strerror(EC_TO_SYS_ERROR(currentStatus));
  } else {
    errorMessage = getProgramError(currentStatus);
  }

  printf("%s\n", errorMessage);
  return EXIT_FAILURE;
}
