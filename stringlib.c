#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "stringlib.h"

// Note: This function returns a pointer to a substring of the original string.
// If the given string was allocated dynamically, the caller must not overwrite
// that pointer with the returned value, since the original pointer must be
// deallocated using the same allocator with which it was allocated.  The return
// value must NOT be deallocated using free() etc.
char *trim(char *str) {
	char *end;

	// Trim leading space
	while(isspace((unsigned char)*str)) str++;

	if(*str == 0)  // All spaces?
		return str;

	// Trim trailing space
	end = str + strlen(str) - 1;
	while(end > str && isspace((unsigned char)*end)) end--;

	// Write new null terminator
	*(end+1) = 0;

	return str;
}

int split(char* string, char* delim, char** result, int *count) {

	//char buf[] =" abc / qwe / ccd ";
	int i = 0;
	char *p = strtok(string, delim);
	//char *array[100];

	while (p != NULL) {
		p = trim(p);
		result[i++] = p;
		p = strtok (NULL, delim);
	}
	
	*count = i;
	return 0;
}

/**
 * check string for allwoed characters
 *
 * return  1 if string contains a character that is not found in allowed.
 * returns 0 if string does only contain characters found in allowed.
 *
 */
int findchar(const char* allowed, const char* string) {
	int leno = strlen(string);
	int leni = strlen(allowed);
	int o, i;
	for (o=0; o<leno; o++) {
		char c = string[o];
		int found = 0;
		//printf("ya\n");
		
		for (i=0; i<leni; i++) {
			if (c == allowed[i])
				found = 1;
		}
		if (found == 0)
			return 1;
	}
	
	return 0;
}

/**
 * find a command line parameter search in a string array (options)
 *
 * since search may be a par of --param=value, compare only the part in 
 * front of the equal sign.
 *
 * Returns -1 if there is no match. Otherwise returns the position of the 
 * first character that matched. Typically this should be 0 if search is 
 * found at the beginning of an element in options.
 */
int find_param(char* search, char** options) {
	
	return -1;
}


/*
int main () {
	char buf[] ="abc/qwe/ccd";
	char *array[3];
	int res;
	
	res = split(buf, "/", array);
	
	if (res !=0)
		return 1;
	
	int i;
	for (i = 0; i < 3; ++i) 
		printf("'%s'\n", array[i]);
	
	
	/ *
	int i = 0;
	char *p = strtok (buf, "/");
	char *array[3];

	while (p != NULL) {
		array[i++] = p;
		p = strtok (NULL, "/");
	}

	for (i = 0; i < 3; ++i) 
		printf("%s\n", array[i]);
	* /
	return 0;
}
*/