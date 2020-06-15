#include <stdlib.h>
#include <string.h>

#include "tree.h"

char* strDoop(const char* str) {
  int len = strlen(str);
  char* newStr = calloc(len + 1, sizeof(char));
  memcpy(newStr, str, len);
  return newStr;
}

symbolNode *createNode(const char* sym, treeData data, int isLabel) {
    symbolNode *root = calloc(1, sizeof(symbolNode));

    root->symbol = strDoop(sym);

    if(isLabel) {
      root->data.address = data.address;
      root->isLabel = 1;
    } else {
      root->data.assemblyLine = data.assemblyLine;
    }
    return root;
}

symbolNode *insert(symbolNode* root, char* sym, treeData data, int isLabel) {
  if(root == NULL) {
    return createNode(sym, data, isLabel);
  } else if(strcmp(root->symbol, sym) > 0) {
    root->left = insert(root->left, sym, data, isLabel);
  } else if(strcmp(root->symbol, sym) < 0) {
    root->right = insert(root->right, sym, data, isLabel);
  } else {
    /* All data is declared on heap by getDataFromOperation,
       if the mnemonic is already inserted in the tree this data is redundant so is freed */
    if(!isLabel) {
      free(data.assemblyLine);
    }
  }
  return root;
}

symbolNode *search(symbolNode *root, char* sym) {

  if(!root || strcmp(root->symbol, sym) == 0) {
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
  if (!root->isLabel) {
    free(root->data.assemblyLine);
  }
  free(root->symbol);
  free(root);
}

assemblyInstruction *getDataFromOperation(char *operation) {
  assemblyInstruction *out = malloc(sizeof(assemblyInstruction));
  if (strcmp(operation, "add") == 0) {
    out->conditionCode = al;
    out->type = DATA_PROCESSING;
    out->code = ADD;
  } else if (strcmp(operation, "sub") == 0) {
    out->conditionCode = al;
    out->type = DATA_PROCESSING;
    out->code = SUB;
  } else if (strcmp(operation, "rsb") == 0) {
    out->conditionCode = al;
    out->type = DATA_PROCESSING;
    out->code = RSB;
  } else if (strcmp(operation, "and") == 0) {
    out->conditionCode = al;
    out->type = DATA_PROCESSING;
    out->code = AND;
  } else if (strcmp(operation, "eor") == 0) {
    out->conditionCode = al;
    out->type = DATA_PROCESSING;
    out->code = EOR;
  } else if (strcmp(operation, "orr") == 0) {
    out->conditionCode = al;
    out->type = DATA_PROCESSING;
    out->code = ORR;
  } else if (strcmp(operation, "mov") == 0) {
    out->conditionCode = al;
    out->type = DATA_PROCESSING;
    out->code = MOV;
  } else if (strcmp(operation, "tst") == 0) {
    out->conditionCode = al;
    out->type = DATA_PROCESSING;
    out->code = TST;
  } else if (strcmp(operation, "teq") == 0) {
    out->conditionCode = al;
    out->type = DATA_PROCESSING;
    out->code = TEQ;
  } else if (strcmp(operation, "cmp") == 0) {
    out->conditionCode = al;
    out->type = DATA_PROCESSING;
    out->code = CMP;
  } else if (strcmp(operation, "mul") == 0) {
    out->conditionCode = al;
    out->type = MULTIPLY;
    out->code = MUL;
  } else if (strcmp(operation, "mla") == 0) {
    out->conditionCode = al;
    out->type = MULTIPLY;
    out->code = MLA;
  } else if (strcmp(operation, "ldr") == 0) {
    out->conditionCode = al;
    out->type = DATA_TRANSFER;
    out->code = LDR;
  } else if (strcmp(operation, "str") == 0) {
    out->conditionCode = al;
    out->type = DATA_TRANSFER;
    out->code = STR;
  } else if (strcmp(operation, "beq") == 0) {
    out->conditionCode = eq;
    out->type = BRANCH;
  } else if (strcmp(operation, "bne") == 0) {
    out->conditionCode = ne;
    out->type = BRANCH;
  } else if (strcmp(operation, "bge") == 0) {
    out->conditionCode = ge;
    out->type = BRANCH;
  } else if (strcmp(operation, "blt") == 0) {
    out->conditionCode = lt;
    out->type = BRANCH;
  } else if (strcmp(operation, "bgt") == 0) {
    out->conditionCode = gt;
    out->type = BRANCH;
  } else if (strcmp(operation, "ble") == 0) {
    out->conditionCode = le;
    out->type = BRANCH;
  } else if (strcmp(operation, "b") == 0) {
    out->conditionCode = al;
    out->type = BRANCH;
  } else if (strcmp(operation, "lsl") == 0) {
    out->conditionCode = al;
    out->type = SHIFT;
    out->code = MOV;
  } else if (strcmp(operation, "andeq") == 0) {
    out->conditionCode = eq;
    out->type = HALT;
    out->code = AND;
  } else {
    /* Invalid mnemonic detected */
    return NULL;
  }
  return out;
}
