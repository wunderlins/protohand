#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// proto:[//]authority[/path][?name=ferret[&n1=v1]][#fragment]
#define NUM_URI_PARTS 6

struct t_uri {
	char *uri;        // original uri
	char *proto;      // protocol part
	char *authority;  // 
	char *path;       // 
	char *query;      // 
	char *fragment;   // 
	
	int pos[NUM_URI_PARTS]; // -1 == unset
};


int main(int argc, char *argv[]) {
	
	// parse an example uri onto the following parts
	//char *uri = "proto://authority/path?name=ferret&n2=v2#fragment";
	char *uri = "proto://authority?query";
	
	int stage = 0;
	int i = 0;
	int length = strlen(uri);
	
	
	char *empty = "";
	struct t_uri uri_parsed = {empty, empty, empty, empty, empty, empty, {-1, -1, -1, -1, -1, -1}};
	uri_parsed.pos[0] = 0;
	
	#define FOUND_START      0
	#define FOUND_PROTO      1
	#define FOUND_AUTHORITY  2
	#define FOUND_PATH       3
	#define FOUND_QUERY      4
	#define FOUND_FRAGMENT   5
	
	for (i=0; i<length; i++) {
		
		printf("[%04d] %c %d", i, uri[i], stage);
		if (stage < FOUND_PROTO && uri[i] == ':') {
			stage = FOUND_PROTO;
			uri_parsed.pos[FOUND_PROTO] = i;
			printf(", found proto");
			if (uri[i+1] == '/' && uri[i+2] == '/') {
				printf(", have double slash");
				i=i+2;
			}
			printf("\n");
			continue;
		} 
		
		if (stage < FOUND_AUTHORITY && (uri[i] == '/' || length == i+1)) {
			stage = FOUND_AUTHORITY;
			uri_parsed.pos[FOUND_AUTHORITY] = i;
			if (length == i+1)
				uri_parsed.pos[FOUND_AUTHORITY]++;
			printf(", found authority");
			printf("\n");
			continue;
		}
		
		if (stage < FOUND_PATH && (uri[i] == '?' || length == i+1)) {
			stage = FOUND_PATH;
			uri_parsed.pos[FOUND_PATH] = i;
			if (length == i+1) {
				uri_parsed.pos[FOUND_PATH]++;
			}
			printf(", found path");
			printf("\n");
			continue;
		}
		
		if (stage < FOUND_QUERY && (uri[i] == '/' || length == i+1)) {
			stage = FOUND_QUERY;
			uri_parsed.pos[FOUND_QUERY] = i;
			if (length == i+1) {
				uri_parsed.pos[FOUND_QUERY]++;
				if (uri_parsed.pos[FOUND_AUTHORITY] == -1)
					uri_parsed.pos[FOUND_AUTHORITY] = i;
			}
			printf(", found query");
			printf("\n");
			continue;
		}
		
		if (stage < FOUND_FRAGMENT && (uri[i] == '/' || length == i+1)) {
			stage = FOUND_FRAGMENT;
			uri_parsed.pos[FOUND_FRAGMENT] = i;
			if (length == i+1)
				uri_parsed.pos[FOUND_FRAGMENT]++;
			printf(", found fragment");
			printf("\n");
			continue;
		}
		
		printf("\n");
		
	}
	
	printf("Start     [%d]\n", uri_parsed.pos[FOUND_START]);
	printf("Proto     [%d]\n", uri_parsed.pos[FOUND_PROTO]);
	printf("Authority [%d]\n", uri_parsed.pos[FOUND_AUTHORITY]);
	printf("Path      [%d]\n", uri_parsed.pos[FOUND_PATH]);
	printf("Query     [%d]\n", uri_parsed.pos[FOUND_QUERY]);
	printf("Fragment  [%d]\n", uri_parsed.pos[FOUND_FRAGMENT]);
	
	return 0;
}