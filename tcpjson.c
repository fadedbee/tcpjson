#include <signal.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>

#include "handle.h"

int main(int argc, const char **argv) {
	struct sockaddr_in myaddr, clientaddr;
	int serverfd;
	static int counter=0;

	printf("tcpjson started...\n");

	serverfd=socket(AF_INET, SOCK_STREAM, 0);
	memset(&myaddr, '0', sizeof(myaddr));
	myaddr.sin_family = AF_INET;
	myaddr.sin_port = htons(2222);
	myaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	if (serverfd < 0) {
		perror("socket");
	}
	if (setsockopt(serverfd, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int)) < 0) {
		perror("setsockopt(SO_REUSEADDR) failed");
	}
	unsigned int len = sizeof(myaddr);
	if (bind(serverfd, (struct sockaddr*) &myaddr, len) < 0) {
		perror("bind");
	}
	if (listen(serverfd, 10) < 0) {
		perror("listen");
	}

	for (;;) {
		int clientfd = accept(serverfd, (struct sockaddr *)&clientaddr, &len);
		int pid = fork(); // returns 0 to child, >0 to parent (the child's pid) 

		if (pid < 0) {
			perror("fork");
			close(clientfd);
		} else if (pid > 0) {
			printf("parent\n");
			close(clientfd);
			counter++;
			continue;
		} else if (pid == 0) {
			printf("child\n");
			handle(clientfd);
			close(clientfd);
			break;
		}
	}

	close(serverfd);
	return 0;
}
