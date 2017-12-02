CC=gcc
CFLAGS=-I. -Wall -Werror -Wpedantic -DJSMN_PARENT_LINKS
DEPS = makefile handle.h jsmn.h readline.h
OBJ = tcpjson.o handle.o jsmn.o readline.o

%.o: %.c $(DEPS)
		$(CC) -c -o $@ $< $(CFLAGS)

tcpjson: $(OBJ)
		gcc -o $@ $^ $(CFLAGS)

.PHONY: clean
clean:
		rm -f *.o tcpjson
