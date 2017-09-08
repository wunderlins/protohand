#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char separator() {
#ifdef _WIN32
	return '\\';
#else
	return '/';
#endif
}

int i;

/**
 * remove ".." from path
 *
 * returns 0 if there was nothing to do. returns 1 if some issues could 
 * be resolved. returns -1 on unsolved issues.
 */
int clean_path(char* path) {
	char sep = separator();
	int retval = 0;
	char* result = malloc(sizeof(char *) * strlen(path));
	
	// traverse trough string.
	int length = strlen(path);
	int found = 0;
	
	for (i=0; i<length; i++) {
		found = 0;
		if (path[i] == '.') {
			// check if this is the beginning of '../'
			if (i+2<length && path[i+1] == '.' && path[i+2] == sep) {
				found = 1;
				if (i == 0)
					return -1;
			}
			
			// check if the last 2 characters aer ".."
			if (length-2 == i && path[i] == '.' && path[i+1] == '.') {
				found = 1;
			}
		}
		
		if (found == 1)
			retval = 1;
		
		printf("[%c] found %d\n", path[i], found);
	}
	
	return retval;
}

int main(int argc, char** argv) {
	char* path = "c:\\windows\\..\\windows\\..";
	char* path2 = "..\\windows";
	int ret;
	ret = clean_path(path);
	printf("Result: %d\n", ret);
	ret = clean_path(path2);
	printf("Result: %d\n", ret);
	return 0;	
}