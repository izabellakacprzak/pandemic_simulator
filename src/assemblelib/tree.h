#ifndef TREE
#define TREE

#include <stdint.h>
#include "../combinedlib/combined_utils.h"

typedef struct assemblyInstruction {
  ConditionCode conditionCode;
  InstructionType type;
  opcode code;
} assemblyInstruction;

typedef union treeData {
  Address address;
  assemblyInstruction *assemblyLine;
} treeData;

/* A binary tree structure used for the symbol table */
typedef struct symNodeStruct {
  char* symbol;
  struct symNodeStruct* left;
  struct symNodeStruct* right;
  treeData data;
  int isLabel;
} symbolNode;

/* Inserts a new node into the tree,
   return NULL if node is already inserted */
symbolNode* insert(symbolNode * root, char* sym, treeData data, int isLabel);

/* Search for an item in the tree on the basis of sym,
   return the node containing sym */
symbolNode* search(symbolNode * root, char* sym);

/* Frees all dynamically allocated nodes */
void freeTable(symbolNode *root);

/* Allocates a new node and adds the symbol and address to it */
symbolNode *createNode(char* sym, treeData data, int isLabel);

assemblyInstruction *getDataFromOperation(char *operation);

#endif
