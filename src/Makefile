CC      = gcc
CFLAGS  = -Wall -Ilib -g -D_DEFAULT_SOURCE -std=c99 -pedantic
LDLIBS  = -Llib -lemulateutils -Lassemblelib -lassembleutils
BUILD	= libs emulate assemble

.SUFFIXES: .c .o

.PHONY: all clean

all: $(BUILD)

assemble: assemble.o

emulate: emulate.o

libs:
	cd lib; make
	cd assemblelib; make

clean:
	rm -f $(wildcard *.o)
	rm -f assemble
	rm -f emulate
	cd lib; make clean
	cd assemblelib; make clean
