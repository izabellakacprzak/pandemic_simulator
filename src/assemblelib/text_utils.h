#ifndef TEXT_UTILS
#define TEXT_UTILS

#include <stdio.h>
#include "../combinedlib/combined_utils.h"
#include "assemble_utils.h"

#define IS_ALPHA(c) \
  ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))

#define IS_NUMERIC(c) \
  (c >= '0' && c <= '9')

#define IS_ALPHANUMERIC(c) \
  ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9'))


/* Load the next instruction line into an array of chars (line)
   and tokenise it into the destArray */
int loadNextInstruction(char **destArray, FILE *sourceFile);

/* Write the assembled instruction into the output file */
int writeNextInstruction(Instruction next, FILE *outputFile);

/* Takes in a line, returns:
   0 if the line is not a label
   1 if the line is a label
   -1 if the first character is not alphabetical (the instruction is invalid) */
int isLabel(char **line);

#endif
