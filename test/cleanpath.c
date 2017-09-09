#include <stdlib.h>
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
int clean_path(char* path, char** result) {
	char sep = separator();
	int retval = 0;
	//char* result = malloc(sizeof(char *) * strlen(path));
	
	// traverse trough string.
	int length = strlen(path);
	int found = 0;
	int rewind = 0;
	int backwardsep = 0;
	
	for (i=0; i<strlen(path); i++) {
		length = strlen(path);
		found = 0;
		if (path[i] == '.') {
			// check if this is the beginning of '../'
			if (i+2<length && path[i+1] == '.' && path[i+2] == sep) {
				found = 1;
				if (i == 0) {
					*result = NULL;
					return -1; // we can not resolve a path that starts with '..'
				}
			}
			
			// check if the last 2 characters are ".."
			if (length-2 == i && path[i] == '.' && path[i+1] == '.') {
				//printf("Yep, ..$\n");
				found = 1;
			}
		}
		
		if (found == 1) {
			retval = 1;
		
			// try to resolve it by looping back to the las '\\'
			backwardsep = 0;
			for (rewind=i-2; rewind > 0; rewind--) {
				if (path[rewind]== sep) {
					backwardsep = rewind;
					break;
				}
			}
			
			//printf("sep: %d\n", backwardsep);
			char* newpath_start = malloc(sizeof(char*) * strlen(path)+1);
			char* newpath_end   = malloc(sizeof(char*) * strlen(path)+1);
			strncpy(newpath_start, path, backwardsep+1);
			newpath_start[backwardsep+1] = '\0';
			
			//printf("Length: %d, sep: %d, i: %d\n", length, backwardsep, i);
			if (i+2 < length) {
				strncpy(newpath_end, path+i+3, strlen(path)-i-2);
				newpath_end[strlen(path)-i-3] = '\0';
				//printf("End: '%s'\n", newpath_end);
				strcat(newpath_start, newpath_end);
			}
			//printf("Intermediate: %s, new \\: %d\n", newpath_start, (backwardsep));
			
			path = newpath_start;
			//retval = clean_path(path, result);
			//break;
			
			i=0; // restart scan process
			path = newpath_start;
		}
		
		//printf("[%c] found %d\n", path[i], found);
	}
	
	*result = (char *)malloc(strlen(path)+1);
	strcpy(*result, path);
	//printf("clean_path(): Path %s\n", path);
	return retval;
}

int main(int argc, char** argv) {
	char *result;
	char* path = "c:\\windows\\..\\windows\\..";
	char* path2 = "..\\windows";
	int ret;
	ret = clean_path(path, &result);
	printf("Result: %d, path: %s\n", ret, result);
	printf("====\n");
	ret = clean_path(path2, &result);
	printf("Result: %d, path: %s\n", ret, result);
	return 0;	
}