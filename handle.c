#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include "readline.h"
#include "jsmn.h"

#define MAX_LINE_LEN 4096

int handle(int sockfd) {
	char buf[100];
	char line[MAX_LINE_LEN];
	snprintf(buf, sizeof buf, "handle(%d)\n", sockfd); // FIXME: snprintf will omit final \0 if contents are huge
	printf("%s", buf);
	send(sockfd, buf, strlen(buf), 0);
	for (;;) {
		ssize_t line_len = readline(sockfd, sizeof(line), line);
		printf("rx: %s\n", line);
		if (line_len < 1) break;

		//
		// parse JSON
		//
		jsmn_parser parser;
		jsmn_init(&parser);

		// parse once to find out how much space we need for tokens
		int num_tokens = jsmn_parse(&parser, line, line_len, NULL, 0); 
		jsmntok_t t[num_tokens];
		printf("%zd %d\n", line_len, num_tokens);

		// parse again to do the job
		// TODO: write a "next" interface for jsmn to return one token at a time, to avoid this double-parsing
		jsmn_init(&parser); // re-initialisation is essential, this 
		num_tokens = jsmn_parse(&parser, line, line_len, t, num_tokens); 
		if (num_tokens < 0) perror("failed to parse JSON");
		printf("%zd %d %d %d\n", line_len, num_tokens, t[0].type, JSMN_OBJECT);
		if (num_tokens < 1 || t[0].type != JSMN_OBJECT) perror("object expected");
		printf("token %d %d %d %d %d %d\n", 0, t[0].type, t[0].start, t[0].end, t[0].size, t[0].parent);

		// loop over the keys of the root object to find the method and the path
		int method = -1;
		int path = -1;  // all parts of the path
		int path0 = -1; // the first part of the path
		for (int i = 0; i < num_tokens; i++) {
			printf("token %d %d %d %d %d %d\n", i, t[i].type, t[i].start, t[i].end, t[i].size, t[i].parent);
			/*
			if (jsmn_string_equal(line, &t[i], "method") == 0) {
				printf("- method: %.*s\n", t[i+1].end-t[i+1].start, line + t[i+1].start);
				method = i+1;
				i++;
			} else if (jsmn_string_equal(line, &t[i], "path") == 0) {
				printf("- path: %.*s\n", t[i+1].end-t[i+1].start, line + t[i+1].start);
				path = i+1;
				i++;
			} else if (t[i].type == JSMN_STRING && t[i].parent == path && path0 == -1) {
				path0 = i+1;
			} else*/ {
				printf("Unexpected key: |%.*s|\n", t[i].end-t[i].start, line + t[i].start);
			}
		}
		printf("%d %d %d\n", method, path, path0);
	}
	printf("handling completed\n");
	return 0;
}
