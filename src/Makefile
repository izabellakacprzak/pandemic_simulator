CC      = gcc
CFLAGS  = -Wall -g -D_DEFAULT_SOURCE -std=c99 -pedantic
BUILD	= assemble emulate

.SUFFIXES: .c .o

.PHONY: all clean

all: $(BUILD)

assemble: assemble.o

pipeline_utils.o: pipeline_utils.h

emulate: pipeline_utils.o emulate.o

execute_test: pipeline_utils.o execute_test.o

clean:
	rm -f $(wildcard *.o)
	rm -f assemble
	rm -f emulate
	rm -f execute_test
