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
int main(int argc, char **argv) {
  assert (argc == 3);

  errorCode currentStatus = OK;
  char *source = argv[1], *dest = argv[2];
  FILE *sourceFile, *destFile;
  int eof;
  char **currentLine = calloc((MAX_INSTRUCTION_SIZE + 1)*(MAX_INSTRUCTION_PARAMS + 1), sizeof(char));
  FATAL_SYS((currentLine = NULL));

  Address curr_address = 0;

  // first pass   -> construct dictionary
  sourceFile = fopen(source, "r");
  FATAL_SYS((sourceFile == NULL)); //file loading failed
  eof = loadNextInstruction(currentLine, sourceFile);
  int label;
  
  while (!eof) {
    //check if the line is a label
    label = isLabel(currentLine);
    if (label == 1) {
      //ADD LABEL TO THE DICTIONARY
    } else if (label == -1) {
      //invalid instruction detected
      FATAL_PROG(1,INVALID_INSTRUCTION);
    }
    eof = loadNextInstruction(currentLine, sourceFile);
    curr_address = curr_address + 4; //address incremented by 4
  }

  FATAL_SYS((fclose(sourceFile) != 0));

  // second pass  -> write instructions
  curr_address = 0;

  //source reopened to go back to the top of the file
  sourceFile = fopen(source, "r");
  FATAL_SYS((sourceFile == NULL));
  destFile = fopen(dest, "w");
  FATAL_SYS((destFile == NULL));

  eof = loadNextInstruction(currentLine, sourceFile);
  int result;
  while (!eof) {
    //result assemble(dict, currentLine);
    //binary writer writes to dest
    eof = loadNextInstruction(currentLine, sourceFile);
  }

  FATAL_SYS((fclose(sourceFile) != 0));
  FATAL_SYS((fclose(destFile) != 0));
  return EXIT_SUCCESS;

 fatalError:
  //free any dynamically allocated memory
  free(currentLine);

  //print error message

  char *errorMessage;
  if (EC_IS_SYS_ERROR(currentStatus)) {
    errorMessage = strerror(EC_TO_SYS_ERROR(currentStatus));
  } else {
    errorMessage = getProgramError(currentStatus);
  }

  printf("%s\n", errorMessage);
  return EXIT_FAILURE;
}
