#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include "readline.h"
#include "jsmn.h"
#include "ffbackend.h"

#define MAX_LINE_LEN 4096
#define JSON_PONG "{\"code\": 204}\n"
#define JSON_UNHANDLED "{\"code\": 404, \"body\":\"unhandled path\"}\n"
#define JSON_SZ 4096

int handle(int sockfd) {
	ffbackend_begin(); // FIXME: move this call inside of the other ffbackend_ functions
	char line[MAX_LINE_LEN];
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

		// parse again to do the job
		// TODO: write a "next" interface for jsmn to return one token at a time, to avoid this double-parsing
		jsmn_init(&parser); // re-initialisation is essential, this 
		num_tokens = jsmn_parse(&parser, line, line_len, t, num_tokens); 
		if (num_tokens < 0) perror("failed to parse JSON");
		if (num_tokens < 1 || t[0].type != JSMN_OBJECT) perror("object expected");

		// loop over the keys of the root object to find the method and the path
		int method = -1;
		int path = -1;  // all parts of the path
		int path0 = -1; // the first part of the path
		int path1 = -1; // the first part of the path
		for (int i = 1; i < num_tokens; i++) {
			if (jsmn_string_equal(line, &t[i], "method") == 0) {
				method = ++i;
			} else if (jsmn_string_equal(line, &t[i], "path") == 0) {
				path = ++i;
			} else if (t[i].type == JSMN_STRING && t[i].parent == path && path0 == -1) {
				path0 = i;
			} else if (t[i].type == JSMN_STRING && t[i].parent == path && path1 == -1) {
				path1 = i;
			} else {
				printf("Unexpected key: |%.*s|\n", t[i].end-t[i].start, line + t[i].start);
			}
		}

		char json[JSON_SZ]; // space for ffbackend to write JSON
		if (jsmn_string_equal(line, &t[method], "GET") == 0 && jsmn_string_equal(line, &t[path0], "ping") == 0) {
			write(sockfd, JSON_PONG, sizeof(JSON_PONG)); 
		} else if (jsmn_string_equal(line, &t[method], "GET") == 0 && jsmn_string_equal(line, &t[path0], "stock") == 0) {
			// FIXME: it's not nice to pass strings as pointers and lengths, but to do otherwise causes
			// unneeded allocations.
			ffbackend_get_stock(line + t[path1].start, t[path1].end-t[path1].start, json, sizeof(json));
			write(sockfd, json, strlen(json)); 
		} else {
			printf("unhandled json: %s\n", line);
			write(sockfd, JSON_UNHANDLED, sizeof(JSON_UNHANDLED)); 
		}
	}
	printf("handling completed\n");
	ffbackend_end(); // FIXME: move this call inside of the other ffbackend_ functions
	return 0;
}
