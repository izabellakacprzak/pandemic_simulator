#include <stdlib.h>
#include <string.h>

#include "tree.h"

symbolNode *createNode(char* sym, Address addr) {
    symbolNode *root = calloc(1, sizeof(symbolNode));

    root->symbol = strdup(sym);
    root->address = addr;

    return root;
}

symbolNode *insert(symbolNode* root, char* sym, Address addr) {
  if(root == NULL) {
      return createNode(sym, addr);
  } else if(strcmp(root->symbol, sym) > 0){
      root->left = insert(root->left, sym, addr);
  } else if(strcmp(root->symbol, sym) < 0){
      root->right = insert(root->right, sym, addr);
  } else{
      return NULL;
  }
  return root;
}

symbolNode *search(symbolNode *root, char* sym) {

  if(strcmp(root->symbol, sym) == 0 || root == NULL) {
      return root;
  }else if(strcmp(root->symbol, sym) > 0){
      return search(root->left, sym);
  }else{
      return search(root->right, sym);
  }
}

void freeTable(symbolNode *root) {
  if(root == NULL) {
        return;
  }
  freeTable(root->left);
  freeTable(root->right);
  free(root);
}
