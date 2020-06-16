CC      = gcc
CFLAGS  = -Wall -Ilib -g -D_DEFAULT_SOURCE -std=c99 -pedantic
LDLIBS  = -Llib -lsimulateutils Lgif_output -lgifenc
BUILD	= libs simulate testsuite

.SUFFIXES: .c .o

.PHONY: all clean

all: $(BUILD)

simulate: simulate.o

testsuite: testsuite.o

libs:
	cd lib; make
	cd gif_output; make

clean:
	rm -f $(wildcard *.o)
	rm -f simulate
	rm -f testsuite
	cd lib; make clean
	cd gif_output; make clean
