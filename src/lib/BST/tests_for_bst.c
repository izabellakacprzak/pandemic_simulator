#include <stdio.h>
#include <string.h>
#include "sym_table.h"

int main()
{

  int errors = 0;
  //not the neatest declaration
  symbolNode* symbolTable = NULL;

  symbolTable = insert(symbolTable, "and", 1);
    insert(symbolTable, "eor", 2);
    insert(symbolTable, "sub", 3);

    insert(symbolTable, "cmp", 4);
    insert(symbolTable, "mul", 5);

    insert(symbolTable, "lsl", 6);
    insert(symbolTable, "teq", 7);


  if ((search(symbolTable, "and"))->address != 1) {
    printf("ERROR: wrong lookup value; should be %d\n", 1);
    errors++;
  }
  if ((search(symbolTable, "eor"))->address != 2) {
    printf("ERROR: wrong lookup value should be %d\n", 2);
    errors++;
  }
  if ((search(symbolTable, "sub"))->address != 3) {
    printf("ERROR: wrong lookup value; should be %d\n", 3);
    errors++;
  }
  if ((search(symbolTable, "cmp"))->address != 4) {
    printf("ERROR: wrong lookup value; should be %d\n", 4);
    errors++;
  }
  if ((search(symbolTable, "mul"))->address != 5) {
    printf("ERROR: wrong lookup value; should be %d\n", 5);
    errors++;
  }
  if ((search(symbolTable, "lsl"))->address != 6) {
    printf("ERROR: wrong lookup value; should be %d\n", 6);
    errors++;
  }
  if ((search(symbolTable, "teq"))->address != 7) {
    printf("ERROR: wrong lookup value; should be %d\n", 7);
    errors++;
  }

  if (errors == 0) {
    printf("Passed all tests\n");
  } 
  else {
    printf("Failed %d tests\n", errors);
  }
  
  //done with symbol table
    freeTable(symbolTable);
  
  return 0;
}
