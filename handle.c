#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include "readline.h"

#define MAX_LINE_LEN 4096

int handle(int sockfd) {
	char buf[100];
	char line[MAX_LINE_LEN];
	snprintf(buf, sizeof buf, "handle(%d)\n", sockfd); // FIXME: snprintf will omit final \0 if contents are huge
	printf("%s", buf);
	send(sockfd, buf, strlen(buf), 0);
	for (;;) {
		ssize_t num = readline(sockfd, sizeof(line), line);
		printf("rx: %s\n", line);
		if (num < 1) break;
		if (strcmp("ping", line) == 0) {
			send(sockfd, "pong\n", 5, 0);
		}
	}
	printf("handling completed\n");
	return 0;
}
