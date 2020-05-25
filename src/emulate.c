#include <stdlib.h>
#include <stdint.h>


/* Registers stored as 32 bit unsigned ints:
     0-12  - general use
     13-14 - SP, LR (ignored for this)
     15    - PC
     16    - CPSR
 */

struct state{
  uint32_t arr[17]
}

int main(int argc, char **argv) {

  // Fetch
  
  

  // Decode



  // Execute

  
  return EXIT_SUCCESS;
}
