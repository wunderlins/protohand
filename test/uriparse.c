#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define DEBUG 1

// proto:[//]authority[/path][?name=ferret[&n1=v1]][#fragment]
#define NUM_URI_PARTS 6

struct t_uri {
	char *uri;        // original uri
	char *proto;      // protocol part
	char *authority;  // 
	char *path;       // 
	char *query;      // 
	char *fragment;   // 
	
	int pos[NUM_URI_PARTS+1]; // -1 == unset
};

int parse(char* uri, struct t_uri* uri_parsed, char* empty) {
	int doubleslash = 0;
	int stage = 1;
	int i = 0;
	int length = strlen(uri);
	
	#define FOUND_START      0
	#define FOUND_PROTO      1
	#define FOUND_AUTHORITY  2
	#define FOUND_PATH       3
	#define FOUND_QUERY      4
	#define FOUND_FRAGMENT   5
	#define FOUND_END        6
	
	//struct t_uri uri_parsed = {uri, empty, empty, empty, empty, empty, {-1, -1, -1, -1, -1, -1, -1}};
	uri_parsed->pos[0] = 0;
	uri_parsed->pos[FOUND_END] = length;
	
	for (i=0; i<length; i++) {
		
		printf("[%04d] %c %d\n", i, uri[i], stage);
		if (stage < FOUND_AUTHORITY && uri[i] == ':') {
			stage = FOUND_AUTHORITY;
			uri_parsed->pos[FOUND_PROTO] = i;
			printf(", found proto\n");
			if (uri[i+1] == '/' && uri[i+2] == '/') {
				printf(", have double slash\n");
				i=i+3;
				doubleslash = 2;
			}
			continue;
		}
		      
		if (stage < FOUND_PATH && uri[i] == '/') {
			stage = FOUND_PATH;
			uri_parsed->pos[FOUND_PATH] = i;
			if (length == i+1)
				uri_parsed->pos[FOUND_PATH]++;
			printf(", found path\n");
			continue;
		}

		if (stage < FOUND_QUERY && uri[i] == '?') {
			stage = FOUND_QUERY;
			uri_parsed->pos[FOUND_QUERY] = i;
			if (length == i+1)
				uri_parsed->pos[FOUND_QUERY]++;
			printf(", found query\n");
			continue;
		}

		if (stage < FOUND_FRAGMENT && uri[i] == '#') {
			stage = FOUND_FRAGMENT;
			uri_parsed->pos[FOUND_FRAGMENT] = i;
			if (length == i+1)
				uri_parsed->pos[FOUND_FRAGMENT]++;
			printf(", found fragment\n");
			continue;
		}

	}
	
	// check if we have an authority
	if (uri_parsed->pos[FOUND_AUTHORITY] == -1) {
		if (uri_parsed->pos[FOUND_PATH] != -1)
			uri_parsed->pos[FOUND_AUTHORITY] = uri_parsed->pos[FOUND_PATH];
	}
	
	if (uri_parsed->pos[FOUND_AUTHORITY] == -1) {
		if (uri_parsed->pos[FOUND_QUERY] != -1)
			uri_parsed->pos[FOUND_AUTHORITY] = uri_parsed->pos[FOUND_QUERY];
	}
	
	if (uri_parsed->pos[FOUND_AUTHORITY] == -1) {
		if (uri_parsed->pos[FOUND_FRAGMENT] != -1)
			uri_parsed->pos[FOUND_AUTHORITY] = uri_parsed->pos[FOUND_FRAGMENT];
	}

	if (uri_parsed->pos[FOUND_AUTHORITY] == -1&& 
	    uri_parsed->pos[FOUND_PROTO]+1 < length) {
		uri_parsed->pos[FOUND_AUTHORITY] = length;
	}
	
	// debug output
	printf("Start     [%d]\n", uri_parsed->pos[FOUND_START]);
	printf("Proto     [%d]\n", uri_parsed->pos[FOUND_PROTO]);
	printf("Authority [%d]\n", uri_parsed->pos[FOUND_AUTHORITY]);
	printf("Path      [%d]\n", uri_parsed->pos[FOUND_PATH]);
	printf("Query     [%d]\n", uri_parsed->pos[FOUND_QUERY]);
	printf("Fragment  [%d]\n", uri_parsed->pos[FOUND_FRAGMENT]);
	printf("End       [%d]\n", uri_parsed->pos[FOUND_END]);

	// check for required parameters
	if (uri_parsed->pos[FOUND_PROTO] == -1 ) 
		return FOUND_PROTO;
	
	if (uri_parsed->pos[FOUND_AUTHORITY] == -1 ) 
		return FOUND_AUTHORITY;
	
	// start chopping strings out of the original string
	uri_parsed->proto = malloc(sizeof(char*) * uri_parsed->pos[FOUND_PROTO]+1);
	strncpy(uri_parsed->proto, uri_parsed->uri, uri_parsed->pos[FOUND_PROTO]);
	uri_parsed->proto[uri_parsed->pos[FOUND_PROTO]+1] = '\0';
	
	uri_parsed->authority = malloc(sizeof(char*) * (uri_parsed->pos[FOUND_AUTHORITY]+1));
	strncpy(uri_parsed->authority, 
	        uri_parsed->uri+(uri_parsed->pos[FOUND_PROTO]+doubleslash+1), 
	        uri_parsed->pos[FOUND_AUTHORITY]-uri_parsed->pos[FOUND_PROTO]-doubleslash-1);
	uri_parsed->authority[uri_parsed->pos[FOUND_AUTHORITY]-uri_parsed->pos[FOUND_PROTO]-doubleslash-1] = '\0';
	
	printf("proto:     %s\n", uri_parsed->proto);
	printf("authority: %s\n", uri_parsed->authority);
	
	return 0;	
}

#ifdef DEBUG 
#if DEBUG > 0
int uri_test(char* uri, char* proto, char* authority, char* path, char* query, char* fragment) {
	return 0;
}

int main(int argc, char *argv[]) {
	
	char* empty = "";
	
	// parse an example uri onto the following parts
	char *uri;
	// uri = "proto://authority/path?name=ferret&n2=v2#fragment";
	// uri = "proto://authority?query";
	//uri = "proto://authority#f";
	
	//uri_parsed.pos[0] = 0;
	//uri_parsed.pos[FOUND_END] = length;
	
	int res, r;
	uri = "proto:authority?a";
	struct t_uri uri_parsed = {uri, empty, empty, empty, empty, empty, {-1, -1, -1, -1, -1, -1, -1}};
	res = parse(uri, &uri_parsed, empty);
	r = uri_test(uri, "proto", "authority", "", "a", "");
}
#endif
#endif