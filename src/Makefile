CC      = gcc
CFLAGS  = -Wall -Wextra -Ilib -g -O0 -D_DEFAULT_SOURCE -std=c99 -pedantic
LDLIBS  = -Llib -lemulateutils -Lassemblelib -lassembleutils -Lcombinedlib -lcombinedutils
BUILD	= libs emulate assemble

.SUFFIXES: .c .o

.PHONY: all clean

all: $(BUILD)

assemble: assemble.o

emulate: emulate.o

libs:
	cd lib; make
	cd assemblelib; make
	cd combinedlib; make

clean:
	rm -f $(wildcard *.o)
	rm -f assemble
	rm -f emulate
	cd lib; make clean
	cd assemblelib; make clean
	cd combinedlib; make clean
