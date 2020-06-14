CC      = gcc
CFLAGS  = -Wall -Ilib -g -D_DEFAULT_SOURCE -std=c99 -pedantic
LDLIBS  = -Llib -lsimulateutils
BUILD	= libs simulate

.SUFFIXES: .c .o

.PHONY: all clean

all: $(BUILD)

simulate: simulate.o

libs:
	cd lib; make

clean:
	rm -f $(wildcard *.o)
	rm -f simulate
	cd lib; make clean
