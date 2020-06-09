#include "text_utils.h"
#include "assemble_utils.h"

/* Creates a mask of 1s from start to end */
#define CREATE_MASK(start, end) ((1 << (start + 1)) - (1 << end))

// loads the next instruction line into an array of chars (given)
int loadNextInstruction(char *destArray, FILE *sourceFile) {
  if (!sourceFile) {
    printf("Could not access file");
    return EXIT_FAILURE;
  }

  if (feof(sourceFile)) {
    printf("Could not read new line");
    return -1;
  }
  
  fgets(destArray, MAX_INSTRUCTION_SIZE, sourceFile);
  
  return EXIT_SUCCESS;
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
