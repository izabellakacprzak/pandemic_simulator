#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "combined_utils.h"
#include "assemble_utils.h"

/* Creates a mask of 1s from start to end */
#define CREATE_MASK(start, end) ((1 << (start + 1)) - (1 << end))

// loads the next instruction line into an array of chars (given)
int loadNextInstruction(char *destArray, FILE *sourceFile) {
  return EXIT_FAILURE;
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


/*
  argv[1] -> input file
  argv[2] -> output file
 */
int main(int argc, char **argv) {
  //assert (argc == 3);

  Address curr_address = 0;

  // first pass   -> construct dictionary

  // second pass  -> write instructions

  FILE *output = fopen("output.bin", "wb");
  writeNextInstruction(204, output); 
  return EXIT_SUCCESS;
}
