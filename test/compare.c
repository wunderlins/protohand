#include <stdio.h>
#include <string.h>

const char* allowed = "acbcdefghijklmnopqrstuvwxyz"
                      "ACBCDEFGHIJKLMNOPQRSTUVWXYZ"
                      "0123456789"
                      "-._~"; // RFC3986, 2.3.  Unreserved Characters

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

int main(int argc, char** argv) {
	
	char mystr[50] = "abcdä";
	
	int result;
	result = findchar(allowed, (const char*) &mystr);
	printf("result: %d\n", result);
	
	return 0;
}
