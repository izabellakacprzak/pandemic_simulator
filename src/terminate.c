#include <stdio.h>
#include <stdlib>

#define NUM_OF_REGISTERS 17
#define SIZE_OF_MEM 23

void terminate(struct CurrentState* currentState){
        printf("Refisters:\n");
        uint32_t *regPtr = &currentState->reg0;
        for(int i = 0; i < 17; i++){
                printf("$%d\t:\t%u (%x)", i, *regPtr, *regPtr);
                regPtr++;
        }

        printf("Non-zero memory:\n");
        for(int i = 0; i < MEMORY_SIZE; i++){
                if(currentState->memory[i] != 0){
                        printf("%x: %x", i*4, currentState->memory[i]);
                }
        }
}
