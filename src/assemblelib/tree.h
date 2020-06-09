#ifndef TREE
#define TREE

#include "assemble_utils.h"
#include <stdlib.h>
#include <stdio.h>

#define ALPHABET_SIZE (26)
#define ALPHABET_SIZE (26)
// not sure if something like this is needed
// - but the symbol table will probably be a struct?
typedef struct Dictionary {
  struct Dictionary **children;

  Address address;

  //If set the word is a mnemonic symbol,
  // by default - 0;
  int mnemonicFlag;
} Dictionary;

Dictionary *createDict(void);

void freeDict(Dictionary *dict);

Dictionary *createNode(void);

void freeNode(Dictionary *root);

int find(Dictionary *root, const char *word);

int insert(Dictionary *root, const char *word);

#endif
