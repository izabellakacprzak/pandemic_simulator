CC = gcc
CFLAGS = -Wall -Wno-unused-variable -g -D_DEFAULT_SOURCE -std=c99 -pedantic
LIB = libassembleutils.a
LIBOBJS = tree.o assemble_utils.o text_utils.o combined_utils.h
BUILD = $(LIB)

all: $(BUILD)

$(LIB): $(LIBOBJS)
	ar rcs $(LIB) $(LIBOBJS)

clean:
	/bin/rm -f $(BUILD) *.o core

assemble_utils.o: assemble_utils.h combined_utils.h

text_utils.o: text_utils.h

tree.o: tree.h
