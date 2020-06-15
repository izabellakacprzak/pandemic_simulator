CC = gcc
CFLAGS = -Wall -Wextra -Ilib -g -O0 -D_DEFAULT_SOURCE -std=c99 -pedantic
LDLIBS = -L../combinedlib -lcombinedutils
LIB = libassembleutils.a
LIBOBJS = tree.o text_utils.o assemble_utils.o
BUILD = libs $(LIB)

all: $(BUILD)

$(LIB): $(LIBOBJS)
	ar rcs $(LIB) $(LIBOBJS)

clean:
	cd ../combinedlib; make clean
	/bin/rm -f $(BUILD) *.o core

libs:
	cd ../combinedlib; make

assemble_utils.o: assemble_utils.h

text_utils.o: text_utils.h

tree.o: tree.h
