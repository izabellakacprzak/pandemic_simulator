#include <stdint.h>
#include "combined_utils.h"

#ifndef ASSEMBLE_UTILS_H
#define ASSEMBLE_UTILS_H

#define MAX_INSTRUCTION_LENGTH 511

typedef uint32_t Address;

// not sure if something like this is needed
// - but the symbol table will probably be a struct?
struct Dictionary {
  struct DictionaryTable *next;
  char *label;
  Address address;
} Dictionary;

Instruction assemble(struct Dictionary *symbolTable, const char *nextInstruction);

int contains(char *value, const char **array);

#endif // ASSEMBLE_UTILS_H
