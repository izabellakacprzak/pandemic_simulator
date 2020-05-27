#include <stdio.h>
#include <stdlib>

#define NUM_OF_REGISTERS 17
#define SIZE_OF_MEM 23

void terminate(struct CurrentState* currentState){
	printf("Refisters:\n");
	for(int i = 0; i < NUM_OF_REGISTERS; i++){
		printf("$%d\t:\t%d (%x)", i, currentState.reg0
	}

	printf("Non-zero memory:\n");
	for(int i = 0; i < SIZE_OF_MEM; i++){
		if(currentState.memory[i] != 0){
			printf("%x: %x", i*4, currentState.memory[i]);
		}
	}
}
