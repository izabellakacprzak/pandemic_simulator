#ifndef TREE
#define TREE

#include <stdint.h>

typedef uint32_t Address;

/* A binary tree structure used for the symbol table */
typedef struct symNodeStruct {
  char* symbol;
  Address address;
  struct symNodeStruct* left;
  struct symNodeStruct* right;
} symbolNode;

/* Inserts a new node into the tree,
   return NULL if node is already inserted */
symbolNode* insert(symbolNode * root, char* sym, Address addr);

/* Search for an item in the tree on the basis of sym,
   return the node containing sym */
symbolNode* search(symbolNode * root, char* sym);

/* Frees all dynamically allocated nodes */
void freeTable(symbolNode *root);

/* Allocates a new node and adds the symbol and address to it */
symbolNode *createNode(char* sym, Address addr);

#endif
