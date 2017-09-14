#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifndef DEBUG
	#define DEBUG 1
#endif

#ifndef URIPARSE_DEBUG
	#define URIPARSE_DEBUG 0
#endif

// placeholder for an empty string
char* empty = "";

// proto:[//]authority[/path][?name=ferret[&n1=v1]][#fragment]
#define FOUND_START      0
#define FOUND_PROTO      1
#define FOUND_AUTHORITY  2
#define FOUND_PATH       3
#define FOUND_QUERY      4
#define FOUND_FRAGMENT   5
#define FOUND_END        6
// how many stages?
#define NUM_URI_PARTS    6

struct t_uri {
	char *uri;        // original uri
	char *proto;      // protocol part
	char *authority;  // 
	char *path;       // 
	char *query;      // 
	char *fragment;   // 
	
	int pos[NUM_URI_PARTS+1]; // -1 == unset
};

int parse(char* uri, struct t_uri* uri_parsed) {
	int doubleslash = 0;
	int stage = 1;
	int i = 0;
	int length = strlen(uri);
	
	//struct t_uri uri_parsed = {uri, empty, empty, empty, empty, empty, {-1, -1, -1, -1, -1, -1, -1}};
	uri_parsed->pos[0] = 0;
	uri_parsed->pos[FOUND_END] = length;
	
	for (i=0; i<length; i++) {
		
		#if DEBUG > 3
		printf("[%04d] %c %d\n", i, uri[i], stage);
		#endif
		if (stage < FOUND_AUTHORITY && uri[i] == ':') {
			stage = FOUND_AUTHORITY;
			uri_parsed->pos[FOUND_PROTO] = i;
			#if DEBUG > 3
			printf(", found proto\n");
			#endif
			if (uri[i+1] == '/' && uri[i+2] == '/') {
				#if DEBUG > 3
				printf(", have double slash\n");
				#endif
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
			#if DEBUG > 3
			printf(", found path\n");
			#endif
			continue;
		}

		if (stage < FOUND_QUERY && uri[i] == '?') {
			stage = FOUND_QUERY;
			uri_parsed->pos[FOUND_QUERY] = i;
			if (length == i+1)
				uri_parsed->pos[FOUND_QUERY]++;
			#if DEBUG > 3
			printf(", found query\n");
			#endif
			continue;
		}

		if (stage < FOUND_FRAGMENT && uri[i] == '#') {
			stage = FOUND_FRAGMENT;
			uri_parsed->pos[FOUND_FRAGMENT] = i;
			if (length == i+1)
				uri_parsed->pos[FOUND_FRAGMENT]++;
			#if DEBUG > 3
			printf(", found fragment\n");
			#endif
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
	#if DEBUG > 1
	printf("Start      [%d]\n", uri_parsed->pos[FOUND_START]);
	printf("Proto      [%d]\n", uri_parsed->pos[FOUND_PROTO]);
	printf("Authority  [%d]\n", uri_parsed->pos[FOUND_AUTHORITY]);
	printf("Path       [%d]\n", uri_parsed->pos[FOUND_PATH]);
	printf("Query      [%d]\n", uri_parsed->pos[FOUND_QUERY]);
	printf("Fragment   [%d]\n", uri_parsed->pos[FOUND_FRAGMENT]);
	printf("End        [%d]\n", uri_parsed->pos[FOUND_END]);
	#endif

	// check for required parameters
	if (uri_parsed->pos[FOUND_PROTO] == -1 ) 
		return FOUND_PROTO;
	
	if (uri_parsed->pos[FOUND_AUTHORITY] == -1 ) 
		return FOUND_AUTHORITY;
	
	// start chopping strings out of the original string
	uri_parsed->proto = malloc(sizeof(char*) * uri_parsed->pos[FOUND_PROTO]+1);
	strncpy(uri_parsed->proto, uri_parsed->uri, uri_parsed->pos[FOUND_PROTO]);
	uri_parsed->proto[uri_parsed->pos[FOUND_PROTO]] = '\0';
	
	uri_parsed->authority = malloc(sizeof(char*) * (uri_parsed->pos[FOUND_AUTHORITY]+1));
	strncpy(uri_parsed->authority, 
	        uri_parsed->uri+(uri_parsed->pos[FOUND_PROTO]+doubleslash+1), 
	        uri_parsed->pos[FOUND_AUTHORITY]-uri_parsed->pos[FOUND_PROTO]-doubleslash-1);
	uri_parsed->authority[uri_parsed->pos[FOUND_AUTHORITY]-uri_parsed->pos[FOUND_PROTO]-doubleslash-1] = '\0';
	
	#if DEBUG > 1
	printf("proto:     %s\n", uri_parsed->proto);
	printf("authority: %s\n", uri_parsed->authority);
	#endif
	
	int len;
	if (uri_parsed->pos[FOUND_PATH] != -1) {
		
		if (uri_parsed->pos[FOUND_QUERY] != -1)
			len = uri_parsed->pos[FOUND_QUERY] - uri_parsed->pos[FOUND_PATH];
		else if (uri_parsed->pos[FOUND_FRAGMENT] != -1)
			len = uri_parsed->pos[FOUND_FRAGMENT] - uri_parsed->pos[FOUND_PATH];
		else
			len = length - uri_parsed->pos[FOUND_PATH];
		
		uri_parsed->path = malloc(sizeof(char*) * (length+1));
		strncpy(uri_parsed->path, 
				uri_parsed->uri+uri_parsed->pos[FOUND_PATH]+1, // skip '/'
				len);
		uri_parsed->path[len-1] = '\0';
	}
	#if DEBUG > 1
	printf("path:      %s\n", uri_parsed->path);
	#endif
	
	if (uri_parsed->pos[FOUND_QUERY] != -1) {
		
		if (uri_parsed->pos[FOUND_FRAGMENT] != -1)
			len = uri_parsed->pos[FOUND_FRAGMENT] - uri_parsed->pos[FOUND_QUERY];
		else
			len = length - uri_parsed->pos[FOUND_QUERY];
		
		uri_parsed->query = malloc(sizeof(char*) * (length+1));
		strncpy(uri_parsed->query, 
				uri_parsed->uri+uri_parsed->pos[FOUND_QUERY]+1, // skip '?'
				len);
		uri_parsed->query[len-1] = '\0';
	}
	#if DEBUG > 1
	printf("query:     %s\n", uri_parsed->query);
	#endif
	
	if (uri_parsed->pos[FOUND_FRAGMENT] != -1) {
		
		len = length - uri_parsed->pos[FOUND_FRAGMENT];
		
		uri_parsed->fragment = malloc(sizeof(char*) * (length+1));
		strncpy(uri_parsed->fragment, 
				uri_parsed->uri+uri_parsed->pos[FOUND_FRAGMENT]+1, // skip '#'
				len);
		uri_parsed->fragment[len-1] = '\0';
	}
	#if DEBUG > 1
	printf("fragment:  %s\n", uri_parsed->fragment);
	#endif
	
	return 0;	
}

#ifdef URIPARSE_DEBUG
#if URIPARSE_DEBUG > 0
int uriparse_test(char* uri, char* proto, char* authority, char* path, char* query, char* fragment) {
	int ret = 0;
	int res;
	struct t_uri uri_parsed = {uri, empty, empty, empty, empty, empty, {-1, -1, -1, -1, -1, -1, -1}};
	res = parse(uri, &uri_parsed);
	if (res != 0) {
		ret = 127+res;
		printf("Parser Error %d, %s\n", res, uri);
		return ret;
	}

	if (strcmp(uri_parsed.proto, proto) != 0)
		ret = FOUND_PROTO;
	
	if (strcmp(uri_parsed.authority, authority) != 0)
		ret = FOUND_AUTHORITY;
	
	if (strcmp(uri_parsed.path, path) != 0)
		ret = FOUND_PATH;
	
	if (strcmp(uri_parsed.query, query) != 0)
		ret = FOUND_PATH;
	
	if (strcmp(uri_parsed.query, query) != 0)
		ret = FOUND_QUERY;
	
	if (strcmp(uri_parsed.fragment, fragment) != 0)
		ret = FOUND_FRAGMENT;

	if (ret == 0) {
		printf("OK     %s\n", uri);
	} else {
		printf("Failed %s\n", uri);
		printf("%-10s, %03d, check: '%s', result: '%s'\n", "proto", strcmp(uri_parsed.proto, proto), proto, uri_parsed.proto);
		printf("%-10s, %03d, check: '%s', result: '%s'\n", "authority", strcmp(uri_parsed.authority, authority), authority, uri_parsed.authority);
		printf("%-10s, %03d, check: '%s', result: '%s'\n", "path", strcmp(uri_parsed.path, path), path, uri_parsed.path);
		printf("%-10s, %03d, check: '%s', result: '%s'\n", "query", strcmp(uri_parsed.query, query), query, uri_parsed.query);
		printf("%-10s, %03d, check: '%s', result: '%s'\n", "fragment", strcmp(uri_parsed.fragment, fragment), fragment, uri_parsed.fragment);
	}
	
	return ret;
}

int main(int argc, char *argv[]) {
	
	// unit testing of the uri parser
	uriparse_test("proto:authority", "proto", "authority", empty, empty, empty);
	uriparse_test("proto:authority/p", "proto", "authority", "p", empty, empty);
	uriparse_test("proto:authority?q", "proto", "authority", empty, "q", empty);
	uriparse_test("proto:authority#f", "proto", "authority", empty, empty, "f");
	uriparse_test("proto:authority?q#f", "proto", "authority", empty, "q", "f");
	uriparse_test("proto:authority/p#f", "proto", "authority", "p", empty, "f");
	uriparse_test("proto:authority/p?q", "proto", "authority", "p", "q", empty);
	uriparse_test("proto:authority/p?a#f", "proto", "authority", "p", "a", "f");
	
}
#endif
#endif