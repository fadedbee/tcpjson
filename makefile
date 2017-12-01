CC=gcc
CFLAGS=-I.
DEPS = 
OBJ = tcpjson.o 

%.o: %.c $(DEPS)
		$(CC) -c -o $@ $< $(CFLAGS)

tcpjson: $(OBJ)
		gcc -o $@ $^ $(CFLAGS)

