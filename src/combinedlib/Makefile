CC = gcc
CFLAGS = -Wall -Wextra -fsanitize=undefined -g -D_DEFAULT_SOURCE -std=c99 -pedantic
LIB = libcombinedutils.a
LIBOBJS = combined_utils.o
BUILD = $(LIB)

all: $(BUILD)

$(LIB): $(LIBOBJS)
	ar rcs $(LIB) $(LIBOBJS)

combined_utils.o: combined_utils.h

clean:
	/bin/rm -f $(BUILD) *.o core
