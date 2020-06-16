CC = gcc
CFLAGS = -Wall -g -D_DEFAULT_SOURCE -std=c99 -pedantic
LIB = libgifenc.a
LIBOBJS = gifenc.o
BUILD = $(LIB)

all: $(BUILD)

$(LIB): $(LIBOBJS)
	ar rcs $(LIB) $(LIBOBJS)

gifenc.o: gifenc.h

clean:
	/bin/rm -f $(BUILD) *.o core
