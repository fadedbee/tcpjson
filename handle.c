#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

int handle(int sockfd) {
	char buf[100];
	snprintf(buf, sizeof buf, "handle(%d)\n", sockfd); // FIXME: snprintf will omit final \0 if contents are huge
	printf("%s", buf);
	send(sockfd, buf, strlen(buf), 0);
	return 0;
}
