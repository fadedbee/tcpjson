#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

// FIXME: horribly inefficient, but not an issue for PoC
// TODO: implement with a buffer and multi-byte reads
// TODO: use ioctl: https://stackoverflow.com/a/3054519/129805
ssize_t readline(int fd, size_t buflen, char *buf) {
	ssize_t i;
	for (i = 0; i < buflen; i++) {
		ssize_t num = read(fd, buf + i, 1);
		// printf("read: %zi %i %c\n", num, buf[i], buf[i]);
		if (num < 1) { 
			i = 0;
			break; // discard last line if unterminated
		}
		// FIXME: a "\r\n" line ending will return an extra empty line
		// TODO: fix with a state machine
		if (buf[i] == '\n') break;
		if (buf[i] == '\r') break; 
	}
	buf[i] = '\0'; // terminate string
	return i;
}
