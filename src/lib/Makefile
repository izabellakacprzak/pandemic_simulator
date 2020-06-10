CC = gcc
CFLAGS = -Wall -Ilib -g -D_DEFAULT_SOURCE -std=c99 -pedantic
LDLIBS = -L../combinedlib -lcombinedutils
LIB = libemulateutils.a
LIBOBJS = execute_utils.o pipeline_utils.o
BUILD = libs $(LIB)

all: $(BUILD)

$(LIB): $(LIBOBJS)
	ar rcs $(LIB) $(LIBOBJS)

libs:
	cd ../combinedlib; make

clean:
	/bin/rm -f $(BUILD) *.o core
	cd ../combinedlib; make clean

pipeline_utils.o: pipeline_utils.h

execute_utils.o: execute_utils.h
