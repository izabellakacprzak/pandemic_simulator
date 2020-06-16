CC = gcc
CFLAGS = -Wall -g -D_DEFAULT_SOURCE -std=c99 -pedantic
LIB = libgifenc.a
LIBOBJS = gifenc.o gif_output.o
BUILD = $(LIB)

all: $(BUILD)

$(LIB): $(LIBOBJS)
	ar rcs $(LIB) $(LIBOBJS)

gifenc.o: gifenc.h

gif_output.o: gif_output.h

clean:
	/bin/rm -f $(BUILD) *.o core
