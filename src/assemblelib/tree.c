#include "tree.h"

Dictionary *create_node(void) {

  Dictionary *node = calloc(1, sizeof *node);
  if(node == NULL){
    perror("Node allocation failed");
    exit(EXIT_FAILURE);
  }

  node -> children = calloc(ALPHABET_SIZE, sizeof (*node->children));
  if(!node-> children){
    perror("Children array allocation failed");
    exit(EXIT_FAILURE);
  }
  node -> mnemonicFlag = 0;

  return node;
}

void free_node(Dictionary *root) {

  if(!root){
    return;
  }

  for(int i = 0; i < ALPHABET_SIZE; i++){
    free_node(root->children[i]);
  }

  free(root -> children);
  free(root);
}

Dictionary *createDictionary(void){
  return create_node();
}

void freeDict(Dictionary *root){
  free_node(root);
}

int find(Dictionary *root, const char *word) {
  if(!root){
    return 0;
  }
  if(!*word){
    return root -> mnemonicFlag;
  }
  if(!IS_ALPHA(*word)){
    return 0;
  }
  if(!root->children[POS(*word)]){
    return 0;
  }
  return find(root->children[POS(*word)], word + 1);
}

int insert(Dictionary *root, const char *word) {

  if(!*word){
    return root -> mnemonicFlag = 1;
  }
  if(!IS_ALPHA(*word)){
    return 0;
  }
  if(!root->children[POS(*word)]){
    root->children[POS(*word)] = create_node();
  }
  return insert(root->children[POS(*word)], word + 1);

}
