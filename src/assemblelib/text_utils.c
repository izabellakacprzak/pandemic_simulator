#include <stdlib.h>
#include <string.h>

#include "text_utils.h"

/* Creates a mask of 1s from start to end */
#define CREATE_MASK(start, end) ((1 << (start + 1)) - (1 << end))

// takes an instruction and leaves it in the form:
//   char** = {label, arg1, arg2 ...}
//  > assumes line is in correct format
//  > destArray may not be the same size as before,
//      now is char destArray[MAX_INSTRUCTION_PARAMS][MAX_INSTRUCTION_SIZE]
//  > destArray should be on the heap
static int instructionTok(char **destArray, const char *line) {
  char **newDestArray = calloc((MAX_INSTRUCTION_SIZE + 1)*(MAX_INSTRUCTION_PARAMS + 1), sizeof(char));

  if (!newDestArray) {
    return EXIT_FAILURE;
  }

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
  
  free(destArray);
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
    free(destArray);
    destArray = NULL;
    return EXIT_SUCCESS;
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

int isLabel(char *line) {
  char *currentChar = line;
  int label = 0;
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
    return -1;
  }
  return label;
}
