#include <signal.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, const char **argv) {
	struct sockaddr_in myaddr, clientaddr;
	int sockfd, newsockfd;
	int pid,new;
	static int counter=0;

	printf("tcpjson started...\n");

	sockfd=socket(AF_INET, SOCK_STREAM, 0);
	memset(&myaddr, '0', sizeof(myaddr));
	myaddr.sin_family = AF_INET;
	myaddr.sin_port = htons(2222);
	myaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	if (sockfd < 0) {
		perror("socket");
	}
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int)) < 0) {
		perror("setsockopt(SO_REUSEADDR) failed");
	}
	unsigned int len = sizeof(myaddr);
	if (bind(sockfd, (struct sockaddr*) &myaddr, len) < 0) {
		perror("bind");
	}
	if (listen(sockfd, 10) < 0) {
		perror("listen");
	}

	for (;;) {
		int new = accept(sockfd, (struct sockaddr *)&clientaddr, &len);
		int pid = fork();

		if (pid == -1) {
			perror("fork");
			close(new);
		} else if (pid > 0) {
			printf("parent\n");
			close(new);
			counter++;
			continue;
		} else if (pid == 0) {
			char buf[100];
			printf("child\n");
			counter++;
			snprintf(buf, sizeof buf, "counter == %d\n", counter);
			send(new, buf, strlen(buf), 0);
			close(new);
			break;
		}
	}

	close(sockfd);
	return 0;
}
