CC = gcc
CFLAGS = -Wall -g -D_DEFAULT_SOURCE -std=c99 -pedantic
LIB = libemulate_utils.a
LIBOBJS = execute_utils.o pipeline_utils.o
BUILD = $(LIB)

all: $(BUILD)

$(LIB): $(LIBOBJS)
	ar rcs $(LIB) $(LIBOBJS)

clean:
	/bin/rm -f $(BUILD) *.o core

pipeline_utils.o: pipeline_utils.h

execute_utils.o: execute_utils.h
