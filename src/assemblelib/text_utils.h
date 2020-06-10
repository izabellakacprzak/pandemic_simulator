#ifndef TEXT_UTILS
#define TEXT_UTILS

#include <stdio.h>

#include "../combinedlib/combined_utils.h"
#include "assemble_utils.h"

#define MAX_INSTRUCTION_SIZE 511
#define MAX_INSTRUCTION_PARAMS 5

int loadNextInstruction(char **destArray, FILE *sourceFile);

int writeNextInstruction(Instruction next, FILE *outputFile);

#define IS_ALPHA(c) \
  ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))

#define IS_NUMERIC(c) \
  (c >= '0' && c <= '9')

#define IS_ALPHANUMERIC(c) \
  ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9'))



int isLabel(char **line);
#endif
