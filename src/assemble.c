#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "combined_utils.h"
#include "assemble_utils.h"

/* Creates a mask of 1s from start to end */
#define CREATE_MASK(start, end) ((1 << (start + 1)) - (1 << end))
#define MAX_INSTRUCTION_SIZE 511
#define MAX_INSTRUCTION_PARAMS 5

// takes an instruction and leaves it in the form:
//   char** = {label, arg1, arg2 ...}
//  > assumes line is in correct format
//  > destArray may not be the same size as before,
//      now is char destArray[MAX_INSTRUCTION_PARAMS][MAX_INSTRUCTION_SIZE]
static int instructionTok(char **destArray, const char *line) {
  char newDestArray[MAX_INSTRUCTION_PARAMS][MAX_INSTRUCTION_SIZE];

  int i = 0;
  while (line[i] != ' ' && line[i] != ':' && line[i] != '\0') {
    i++;
  }

  if (line[i] == '\0') {
    return EXIT_FAILURE;
  }

  // new function label case
  if (line[i] == ':') {
    strncpy(newDestArray[0], line, i + 1);
    return EXIT_SUCCESS;
  }

  strncpy(newDestArray[0], line, i);
  
  int start = i + 1;   // current start place in line
  i = 0;
  int j = 1;           // argument number (destArray[j][])
  while (line[i] != '\0') {
    if (line[i] == ',') {
      strncpy(newDestArray[j], line + start, i);
      start += i + 1;
      i = 0;
      j++;
      
    } else if (line[i] == '[') {
      while (line[i] != ']') {
	i++;
      }
    }

    i++;
  }

  strncpy(newDestArray[j], line + start, i);

  if (j > MAX_INSTRUCTION_PARAMS) {
    return EXIT_FAILURE;
  }

  destArray = newDestArray;
  return EXIT_SUCCESS;
}

// loads the next instruction line into an array of chars (line)
//   then tokenises into the destArray
//  > destArray may not be the same size as before,
//      now is char destArray[MAX_INSTRUCTION_PARAMS][MAX_INSTRUCTION_SIZE]
int loadNextInstruction(char **destArray, FILE *sourceFile) {
  if (!sourceFile) {
    printf("Could not access file");
    return EXIT_FAILURE;
  }

  if (feof(sourceFile)) {
    printf("Could not read new line");
    return -1;
  }

  char line[MAX_INSTRUCTION_SIZE];
  fgets(line, MAX_INSTRUCTION_SIZE, sourceFile);
  
  return instructionTok(destArray, line);
}

int writeNextInstruction(Instruction next, FILE *outputFile) {
	Instruction instruction = (CREATE_MASK(7, 0) & next)
		| (CREATE_MASK(15, 8) & next)
		| (CREATE_MASK(23, 16) & next)
		| (~((1 << 24) - 1)  & next);
	if(!outputFile){
		printf("Could not access file");
		return 1;
	}
	fwrite(&instruction, 4, 1, outputFile);
	return 0;
}

int isAlpha(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'); 
}

int isNumeric(char c) {
  return (c >= '0'&& c <= '9');  
}

int isAlphaNumeric(char c) {
  return isAlpha(c) || isNumeric(c);
}

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
