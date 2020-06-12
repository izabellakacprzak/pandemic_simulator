#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "assemble_utils.h"
#include "text_utils.h"

/* Creates a mask of 1s from start to end */
#define CREATE_MASK(start, end) ((1 << (start + 1)) - (1 << end))

// takes an instruction and leaves it in the form:
//   char** = {label, arg1, arg2 ...}
//  > assumes line is in correct format
//  > destArray may not be the same size as before,
//      now is char destArray[MAX_INSTRUCTION_PARAMS][MAX_INSTRUCTION_SIZE]
//  > destArray should be on the heap
//  > destArray is NULL terminated
static int instructionTok(char **destArray, char *line) {
    assert(strlen(line) <= MAX_INSTRUCTION_SIZE);

    if (!destArray || !line) {
      return INVALID_INSTRUCTION;
    }
    
    destArray[0] = strtok(line, " ");
    
    int i = 0;
    char *rest = NULL;
    while (destArray[i]) {
      i++;
      destArray[i] = strtok_r(rest, " ,", &rest);
      
      if (rest[0] == '[') {
	int j = 1;
	while (rest[j] != ']') {
	  j++;
	}

	strcpy(&rest[j], rest);
	rest[j + 1] = '\0';
      }
    }

    return OK;
}

// loads the next instruction line into an array of chars (line)
//   then tokenises into the destArray
//  > destArray may not be the same size as before,
//      now is char destArray[MAX_INSTRUCTION_PARAMS][MAX_INSTRUCTION_SIZE]
int loadNextInstruction(char **destArray, FILE *sourceFile) {
    if (!sourceFile) {
        printf("Could not access file");
        return SYS;
    }

    if (feof(sourceFile)) {
        free(destArray);
        destArray = NULL;
        return END_OF_FILE;
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

/*Takes in a line, returns:
  0 if the line is not a label
  1 if the line is a label
  -1 if the first character is not alphabetical (the instruction is invalid)
 */
int isLabel(char **line) {
  char *currentChar = *line; //takes first token of the line

  if (IS_ALPHA(*currentChar)){
    //first character of a label must be alphabetical
    while (*(currentChar + 1) != '\0') {
      currentChar++;
      if (IS_ALPHANUMERIC(*currentChar)) {
	continue; //nothing to be done
      } else if (*currentChar == ':') {
	//should be the end of the string
	if (*(currentChar + 1) == '\0'){
	  return 1; //first parameter is alphanumeric, terminated with a ':'
	} else {
	  return 0; //not a valid label as colon in middle of string
	}
      } else {
	return 0; //not a label as there are invalid characters for a label
      }
    }
  } else {
    //all instructions should start with an alphabetical character
    //should result in a fatal program error INVALID_INSTRUCTION
    return -1;
  }

  //no ending ':' found on first parameter so no label
  return 0;
}
