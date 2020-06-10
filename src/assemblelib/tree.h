#ifndef TREE
#define TREE

#include <stdint.h>

typedef uint32_t Address;

typedef struct symNodeStruct {
  char* symbol;
  Address address;
  struct symNodeStruct* left;
  struct symNodeStruct* right;
} symbolNode;

//function prototypes
symbolNode* insert(symbolNode * root, char* sym, Address addr);

symbolNode* search(symbolNode * root, char* sym);

void freeTable(symbolNode *root);

symbolNode *createNode(char* sym, Address addr);

#endif
