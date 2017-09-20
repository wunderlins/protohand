#include <stdlib.h>
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

int find_substr(char* string, char* search) {
	int index = -1;
	char *pch = strstr(string, search);
	if (pch != NULL) {
		index = pch - string;
	}
	return index;
}

int starts_with(const char *pre, const char *str) {
	#if DEBUG > 2
	fprintf(logfile, "search '%s' for '%s'\n", str, pre);
	#endif
    size_t lenpre = strlen(pre),
           lenstr = strlen(str);
    return lenstr < lenpre ? -1 : strncmp(pre, str, lenpre);
}

int get_value_from_argument(char* argument, char* ret) {
	
	int end = strlen(argument);
	int r = strpos(argument, '=');
	
	if (r == -1) {
		strcpy(ret, argument);
		return 0;
	}
	
	// copy value and get rid of trailing white space
	end = end - (r+1);
	argument = argument+r+1;
	while(isspace((unsigned char)*argument)) argument++;
	strncpy(ret, argument, strlen(argument)+1);
	ret[end] = '\0';
	return 1;
}

/**
 * returns the key of a key=value pair
 *
 * returns NULL if nothing is found
 */
char* find_key(char* search) {
	
	char* key;
	int ret = strpos(search, '=');
	
	if (ret == -1) {
		return NULL;
	} 
	
	key = malloc(sizeof(char *) * ret);
	key = strncpy(key, search, ret);
	key[ret] = '\0';
	
	// remove white space at the beginning and end
	key = trim(key);
	
	return key;
}

/**
 * find a command line parameter search in a string array (options)
 *
 * since search may be a par of --param=value, compare only the part in 
 * front of the equal sign.
 *
 * Returns 0 if there is no match. Otherwise returns 1.
 */
int find_param(char* search, struct str_array* options) {

	int res;
	
	char* key;
	int ret = strpos(search, '=');
	
	if (ret == -1) {
		key = search;
	} else {
		key = malloc(sizeof(char *) * ret);
		key = strncpy(key, search, ret);
		key[ret] = '\0';
	}
	
	// remove white space at the beginning and end
	key = trim(key);
	
	#if DEBUG > 2
	fprintf(logfile, "key: %s\n", key);
	#endif

	int counter = 0;
	for (counter=0; counter<options->length; counter++) {
		res = find_substr(options->items[counter], key);
		#if DEBUG > 2
		fprintf(logfile, "--> %s [%d]\n", options->items[counter], res);
		#endif
		
		if (res == 0)
			return 1;
	}

	return 0;
}

/**
str_array with length: https://stackoverflow.com/questions/9522760/find-the-number-of-strings-in-an-str_array-of-strings-in-c

As another way, you could create a struct that is filled with the values you want.

And you would use it as such:

struct str_array mystr_array = str_array("Hi", "There", "This", "Is", "A", "Test");
// use mystr_array

printf("%i", mystr_array.length);
str_array_destroy(mystr_array);

*/

struct str_array str_array_make(char **elements, int count) {
	struct str_array ret;
	ret.items = malloc(sizeof(char *) * count);
	ret.length = count;
	
	int i;
	for (i = 0; i < count; i++) {
		ret.items[i] = elements[i];
	}

	return ret;
}

/**
 * split string by delim.
 *
 * returns a str_array containing items and length
 */
struct str_array str_array_split(char* string, char* delim) {
	char** result = malloc(sizeof(char *) * 512);
	struct str_array ret;

	#if DEBUG > 1
	fprintf(logfile, "delim:  %s\n", delim);
	fprintf(logfile, "string: %s\n", string);
	#endif
	
	//char buf[] =" abc / qwe / ccd ";
	int i = 0;
	char *p = strtok(string, delim);
	//char *array[100];

	while (p != NULL) {
		p = trim(p);
		result[i++] = p;
		p = strtok (NULL, delim);
	}
	
	if (result != NULL) {
		ret.items = result;
		ret.length = i;
	}
	
	// FIXME: dynamically allocate more memory if the array shall be 
	//        longer than 512 elements
	
	return ret;
}

int strpos(char* string, char find) {
	int index = -1;
	const char *ptr = strchr(string, find);
	if(ptr) {
		index = ptr - string;
		// do something
	}
	return index;
}

void urldecode2(char *dst, const char *src) {
	char a, b;
	while (*src) {
		if ((*src == '%') &&
			((a = src[1]) && (b = src[2])) &&
			(isxdigit(a) && isxdigit(b))) {
			if (a >= 'a')
				a -= 'a'-'A';
			if (a >= 'A')
				a -= ('A' - 10);
			else
				a -= '0';
			if (b >= 'a')
				b -= 'a'-'A';
			if (b >= 'A')
				b -= ('A' - 10);
			else
				b -= '0';
			*dst++ = 16*a+b;
			src+=3;
		} else if (*src == '+') {
			*dst++ = ' ';
			src++;
		} else {
			*dst++ = *src++;
		}
	}
	*dst++ = '\0';
}

void hex_dump(char *desc, void *addr, int len)  {
    int i;
    unsigned char buff[17];
    unsigned char *pc = (unsigned char*)addr;

    // Output description if given.
    if (desc != NULL)
        printf ("%s:\n", desc);

    // Process every byte in the data.
    for (i = 0; i < len; i++) {
        // Multiple of 16 means new line (with line offset).

        if ((i % 16) == 0) {
            // Just don't print ASCII for the zeroth line.
            if (i != 0)
                printf("  %s\n", buff);

            // Output the offset.
            printf("  %04x ", i);
        }

        // Now the hex code for the specific character.
        printf(" %02x", pc[i]);

        // And store a printable ASCII character for later.
        if ((pc[i] < 0x20) || (pc[i] > 0x7e)) {
            buff[i % 16] = '.';
        } else {
            buff[i % 16] = pc[i];
        }

        buff[(i % 16) + 1] = '\0';
    }

    // Pad out last line if not exactly 16 characters.
    while ((i % 16) != 0) {
        printf("   ");
        i++;
    }

    // And print the final ASCII bit.
    printf("  %s\n", buff);
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