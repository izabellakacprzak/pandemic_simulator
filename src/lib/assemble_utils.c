#include <stdint.h>
#include "combined_utils.h"
#include "assemble_utils.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "combined_utils.h"
#include "assemble_utils.h"

#define IS_ALPHA(x) (x >= 'a' && x<= 'z')
#define POS(x) (x - 'a')
#define ALPHABET_SIZE (26)


// takes current instruction and sets a register at bit,
//   or just takes bit and can be used as a mask

// set from bit to given value? not sure if a good idea to refactor like that
//however it would be more versatile
static Instruction setBits(uint32_t value, int bit, Instruction instr) {
  return instr |= (value << bit);;
}


Dictionary *createDictionary(void){
    return createNode();
}

void freeDict(Dictionary *root){
    freeNode(root);
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
        root->children[POS(*word)] = createNode();
    }
    return insert(root->children[POS(*word)], word + 1);

}

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

// all of these probably take char *nextInstruction or nothing at all
static Instruction setDataProcessing() {

  Instruction instruction = 0;

    //sets Cond Code
  instruction = setBits(al, 31, instruction);

    //if instruction is supposed to be tst, teq or cmp - set S - bit 20
    // otherwise it is clear (cleared upon initialisation)

  return instruction;
}

static Instruction setMultiply() {
  Instruction instruction = 0;
    //sets Cond Code
  instruction = setBits(al, 31, instruction);
  instruction = setBits(0x9, 7, instruction);

    //check whether mnemonic is mla or mul
    // and set A accordingly
    //on initialisation instruction is set with all bits to 0
    // so there is no need to manually set S


  return instruction;
 }

static Instruction setDataTransfer() {

  Instruction instruction = 0;
  
  instruction = setBits(1, 26, instruction);
  
  return 0;
}

static Instruction setBranch() {

  Instruction  instruction = 0;
    //check the mnemonic suffix for b and execute with that condition
    //as of now set to al(if no suffix - al)
    //sets Cond Code
  instruction = setBits(al, 31, instruction);
    //sets the 101 from 27
  instruction = setBits(0x5, 27, instruction);

  return instruction;
}

static Instruction setHalt() {
  return 0;
}


//declaration was conflicting with the one in the header - added const

Instruction assemble(Dictionary *symbolTable,const char *nextInstruction) {
  return 0;
}
