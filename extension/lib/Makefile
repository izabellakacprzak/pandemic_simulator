CC = gcc
CFLAGS = -Wall -g -D_DEFAULT_SOURCE -std=c99 -pedantic
LIB = libsimulateutils.a
LIBOBJS = simulate_utils.o simulationIO.o simulate_social.o
BUILD = $(LIB)

all: $(BUILD)

$(LIB): $(LIBOBJS)
	ar rcs $(LIB) $(LIBOBJS)

simulate_utils.o: simulate_utils.h

simulationIO.o: simulationIO.h

simulate_social.o: simulate_social.h

clean:
	/bin/rm -f $(BUILD) *.o core

