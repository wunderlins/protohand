#ifndef __URIPARSE_H__
#define __URIPARSE_H__

/* Make this header file easier to include in C++ code */
#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "stringlib.h"
#include "../test/nvlist.h"

// sohw debug output. 0=disabled, debug levels up to 4 are available
#ifndef DEBUG
	#define DEBUG 1
#endif

// compile main and unit test methods if > 0
#ifndef URIPARSE_DEBUG
	#define URIPARSE_DEBUG 0
#endif

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


// struct holding an uri string representation, all elements as separate 
// strings as well as parser positions
struct t_uri {
	char *uri;        // original uri
	char *proto;      // protocol part
	char *authority;  // 
	char *path;       // 
	char *query;      // 
	char *fragment;   // 
	
	int pos[NUM_URI_PARTS+1]; // -1 == unset

	struct nvlist_list nvquery;
};


/**
 * Create empty t_uri struct
 *
 * sets all strings to empty ("") and positions to -1
 */
struct t_uri uriparse_create(char* uri);

/**
 * parse a string into an uri struct
 *
 * The struct supports 
 * proto:[//]authority[/path][?name=ferret[&n1=v1]][#fragment]. Credentials
 * in the authority are ignored (considered part of the authority).
 */
int uriparse_parse(char* uri, struct t_uri* uri_parsed);

#ifdef URIPARSE_DEBUG
#if URIPARSE_DEBUG > 0

/**
 * unit testing method
 *
 * provide an uri string as parameter 1, then the parts of the uri as 
 * subsequent parameters. The method wil parse an uri and the compare parsed 
 * atributes to the remaining parameters. if all parameters 
 * are equal, the method wil lreturn 0.
 */
 
int uriparse_test(char* uri, char* proto, char* authority, char* path, char* query, char* fragment);

#endif
#endif

#ifdef __cplusplus
}
#endif

#endif /* __URIPARSE_H__ */