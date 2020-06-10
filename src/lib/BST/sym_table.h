
//For Symbol table
typedef struct symNodeStruct symbolNode;

struct symNodeStruct {
  char* symbol;
  int address;
  symbolNode* left;
  symbolNode* right;
};

//typedef struct table{
//  symbolNode* root;
//}Table;

//function prototypes
symbolNode* insert(symbolNode * root, char* sym, int addr);
symbolNode* search(symbolNode * root, char* sym);
void freeTable(symbolNode *root);
symbolNode *createNode(char* sym, int addr);
//Table *createTable(void);
//Table *insertTableSymbol(Table *table, char* sym, int addr);
//void freeTable(Table *table);
