#include <stdint.h>
#include "combined_utils.h"

#ifndef ASSEMBLE_UTILS_H
#define ASSEMBLE_UTILS_H

#define MAX_INSTRUCTION_LENGTH 511

typedef uint32_t Address;

// not sure if something like this is needed
// - but the symbol table will probably be a struct?
typedef struct Dictionary {
  struct Dictionary **children;

  Address address;

  //If set the word is a mnemonic symbol,
  // by default - 0;
  int mnemonicFlag;
} Dictionary;

Instruction assemble(struct Dictionary *symbolTable, const char *nextInstruction);

int contains(char *value, const char **array);

Dictionary *createDict(void);

void freeDict(Dictionary *dict);

Dictionary *createNode(void);

void freeNode(Dictionary *root);

int find(Dictionary *root, const char *word);

int insert(Dictionary *root, const char *word);

#endif // ASSEMBLE_UTILS_H

