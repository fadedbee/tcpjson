CDATE=`date +%d-%m-%y`
COMPMAC=`uname -n`
HEADERS=${DEV_HOME}/disam96/head/
RESOURCE=${DEV_HOME}/resource/
FRED=${DEV_HOME}/fred.linux/
#FRED=${DEV_HOME}/fred040820/
ISAM=${DEV_HOME}/disam96/code/libdisam96.elf.a
FFLIB=${FRED}libff.elf.a
STRIP=strip
MV=mv
CC=gcc
LDFLAGS=-lm ${ISAM}

CFLAGS=-I. -Wall -DJSMN_PARENT_LINKS --std=gnu99 -g -O -Wunused -fstrength-reduce -fwritable-strings -DLINUX -DSTDCONFIG -I${DEV_HOME}/source/stock -DDISAM  -I${DEV_HOME}/disam96/head -I${FRED} -I${HEADERS} -I${RESOURCE} -DCOMPMAC=\"${COMPMAC}\" -DCOMPDATE=\"${CDATE}\" -DCCPROG=\"$@\" -I$(NIV_TEST_RESOURCES)
# FIXME: RH4.7 gcc doesn't know -Wpedantic
# FIXME: existing code won't compile with -Werror as it's full of warnings

MSCRCOM = ../common/mscrcom.c ../common/mscrcom.h
NIV_TEST_RESOURCES=${DEV_HOME}/source/resources

DEPS = makefile handle.h jsmn.h readline.h
OBJ = tcpjson.o ffbackend.o handle.o jsmn.o readline.o

%.o: %.c $(DEPS)
		$(CC) -c -o $@ $< $(CFLAGS)

tcpjson: $(OBJ)
		gcc -o $@ $^ $(CFLAGS) $(FFLIB) $(LDFLAGS)

#ffbackend:	ffbackend.c $(OBJ) ${DEV_HOME}/source/common/common.c
#	$(CC) $(CFLAGS) $@.c $(FFLIB) -o $@ $(LDFLAGS)

.PHONY: clean
clean:
		rm -f *.o tcpjson
