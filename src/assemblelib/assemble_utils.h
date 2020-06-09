#include <stdint.h>
#include <errno.h>
#include "combined_utils.h"

#ifndef ASSEMBLE_UTILS_H
#define ASSEMBLE_UTILS_H

#define MAX_INSTRUCTION_LENGTH 511

typedef uint32_t Address;

typedef enum errorCode {OK, INVALID_INSTRUCTION, SYS} errorCode;

typedef struct err {
  int code;
  char *message;
} errorType;

#define EC_FROM_SYS_ERROR(e) (SYS + e)
#define EC_TO_SYS_ERROR(e) (e - SYS)
#define EC_IS_SYS_ERROR(e) (e >= SYS)

//for program errors such as invalid instructions
#define FATAL_PROG(pred, status) \
  do { if (pred) {currentStatus = status; goto fatalError;} } while (0)

//for system errors such as failed file open
#define FATAL_SYS(pred) \
  do { if (pred) {currentStatus = EC_FROM_SYS_ERROR(errno); goto fatalError;} } while (0)


// not sure if something like this is needed
// - but the symbol table will probably be a struct?
typedef struct Dictionary {
  struct Dictionary **children;

  Address address;

  //If set the word is a mnemonic symbol,
  // by default - 0;
  int mnemonicFlag;
} Dictionary;

Instruction assemble(struct Dictionary *symbolTable, const char *nextInstruction);

int contains(char *value, const char **array);

Dictionary *createDict(void);

void freeDict(Dictionary *dict);

Dictionary *createNode(void);

void freeNode(Dictionary *root);

int find(Dictionary *root, const char *word);

int insert(Dictionary *root, const char *word);

char *getProgramError(errorCode e);

#endif // ASSEMBLE_UTILS_H

