#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "combined_utils.h"
#include "assemble_utils.h"

// loads the next instruction line into an array of chars (given)
int loadNextInstruction(char *destArray, FILE *sourceFile) {
  return EXIT_FAILURE;
}

int writeNextInstruction(Instruction next, FILE *outputFile) {
  return EXIT_FAILURE;
}


/*
  argv[1] -> input file
  argv[2] -> output file
 */
int main(int argc, char **argv) {
  assert (argc == 3);

  Address curr_address = 0;

  // first pass   -> construct dictionary

  // second pass  -> write instructions
  
  return EXIT_SUCCESS;
}
