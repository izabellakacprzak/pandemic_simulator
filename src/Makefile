CC      = gcc
CFLAGS  = -Wall -Ilib -g -D_DEFAULT_SOURCE -std=c99 -pedantic
LDLIBS  = -Llib -lemulateutils
BUILD	= libs assemble emulate

.SUFFIXES: .c .o

.PHONY: all clean

all: $(BUILD)

assemble: assemble.o

emulate: emulate.o

libs:
	cd lib; make

clean:
	rm -f $(wildcard *.o)
	rm -f assemble
	rm -f emulate
	cd lib; make clean
