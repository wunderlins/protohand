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