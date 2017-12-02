CC=gcc
CFLAGS=-I. -Wall -Werror -Wpedantic
DEPS = handle.h jsmn.h
OBJ = tcpjson.o handle.o jsmn.o

%.o: %.c $(DEPS)
		$(CC) -c -o $@ $< $(CFLAGS)

tcpjson: $(OBJ)
		gcc -o $@ $^ $(CFLAGS)

.PHONY: clean
clean:
		rm -f *.o tcpjson
