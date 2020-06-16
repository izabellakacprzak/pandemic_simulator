CC      = gcc
CFLAGS  = -Wall -Wextra -Ilib -g -D_DEFAULT_SOURCE -std=c99 -pedantic
LDLIBS  = -Llib -lsimulateutils -Lgifoutput -lgifenc
BUILD	= libs simulate testsuite

.SUFFIXES: .c .o

.PHONY: all clean

all: $(BUILD)

simulate: simulate.o

testsuite: testsuite.o

libs:
	cd lib; make
	cd gifoutput; make

clean:
	rm -f $(wildcard *.o)
	rm -f simulate
	rm -f testsuite
	cd lib; make clean
	cd gifoutput; make clean
