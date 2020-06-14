CC = gcc
CFLAGS = -Wall -g -D_DEFAULT_SOURCE -std=c99 -pedantic
LIB = libsimulateutils.a
LIBOBJS = simulate_utils.o simulationIO.o
BUILD = $(LIB)

all: $(BUILD)

$(LIB): $(LIBOBJS)
	ar rcs $(LIB) $(LIBOBJS)

simulate_utils.o: simulate_utils.h

simulationIO.o: simulationIO.h
clean:
	/bin/rm -f $(BUILD) *.o core
