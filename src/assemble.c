#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "assemblelib/combined_utils.h"
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
  char *currentLine = calloc(512, sizeof(char));
  char *currentChar;
  FATAL_SYS((currentLine = NULL));

  Address curr_address = 0;

  // first pass   -> construct dictionary
  sourceFile = fopen(source, "r");
  FATAL_SYS((sourceFile == NULL)); //file loading failed
  eof = loadNextInstruction(currentLine, sourceFile);
  int label = 0;
  while (!eof) {
    //check if the line is a label
    currentChar = currentLine;
    label = 0;
    if (isAlpha(*currentChar)){
      //first character of a label must be alphabetical
      while (*(currentChar + 1) != '\0') {
	currentChar++;
	if (isAlphaNumeric(*currentChar)) {
	  continue; //nothing to be done
	} else if (*currentChar == ':') {
	  //should be the end
	  if (*(currentChar + 1) == '\0'){
	    label = 1;
	  } else {
	    break; //not a valid label
	  }
	} else {
	  break; //not a valid label
	}
      }
    } else {
      //all instructions should start with an alphabetical character
      FATAL_PROG(1,INVALID_INSTRUCTION);
    }
    if (label) {
      //ADD LABEL TO THE DICTIONARY
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
