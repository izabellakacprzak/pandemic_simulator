CC      = gcc
CFLAGS  = -Wall -Ilib -g -D_DEFAULT_SOURCE -std=c99 -pedantic
LDLIBS  = -Lemulatelib -lemulate_utils
BUILD	= libs assemble emulate

.SUFFIXES: .c .o

.PHONY: all clean

all: $(BUILD)

assemble: assemble.o

emulate:  emulate.o

libs:
	cd emulatelib; make

clean:
	rm -f $(wildcard *.o)
	rm -f assemble
	rm -f emulate
	cd emulatelib; make clean
