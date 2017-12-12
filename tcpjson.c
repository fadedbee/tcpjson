#include <signal.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>

#include "handle.h"

/*
 * The main function creates a listening sockey and forks a child process for each TCP connection.
 */
int main(int argc, const char **argv) {
    struct sockaddr_in myaddr, clientaddr;
    int serverfd;
    static int counter=0;

    printf("tcpjson started...\n");

    serverfd=socket(AF_INET, SOCK_STREAM, 0);
    memset(&myaddr, '0', sizeof(myaddr));
    myaddr.sin_family = AF_INET;
    myaddr.sin_port = htons(2222); // FIXME: look these up from the commandline, or config?
    myaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (serverfd < 0) 
    {
        perror("socket");
        return -1;  // TODO: proper error handling
    }
    if (setsockopt(serverfd, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int)) < 0) 
    {
        perror("setsockopt(SO_REUSEADDR) failed");
        return -1;
    }
    unsigned int len = sizeof(myaddr);
    if (bind(serverfd, (struct sockaddr*) &myaddr, len) < 0) 
    {
        perror("bind");
        return -1;
    }
    if (listen(serverfd, 10) < 0) 
    {
        perror("listen");
        return -1;
    }

    for (;;) 
    {
        int clientfd = accept(serverfd, (struct sockaddr *)&clientaddr, &len);

		/* 
         * Before fork() there is one (parent) process.
         */
        int pid = fork(); 
        /*
         * After fork() there are processes running this same code.
         * 0 is returned to the child and, the child's pid (>0) is returned to the parent. 
         */

        if (pid < 0) 
        {
            perror("fork");
            close(clientfd);
        } 
        else if (pid > 0) 
        {
            printf("parent\n");
            close(clientfd);
            counter++;
            continue;
        } 
        else if (pid == 0) 
        {
            printf("child\n");
            handle(clientfd);
            close(clientfd);
            break;
        }
    }

    close(serverfd); // FIXME: should we close this (in the child process) in the child stanza above?
    return 0;
}
