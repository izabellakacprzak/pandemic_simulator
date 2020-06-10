
#ifndef TEXT_UTILS
#define TEXT_UTILS

#include <stdio.h>
#include "../combinedlib/combined_utils.h"

#define MAX_INSTRUCTION_SIZE 511
#define MAX_INSTRUCTION_PARAMS 5

int loadNextInstruction(char **destArray, FILE *sourceFile);

int writeNextInstruction(Instruction next, FILE *outputFile);

int isAlpha(char c);

int isNumeric(char c);

int isAlphaNumeric(char c);

int isLabel(char *line);
#endif
