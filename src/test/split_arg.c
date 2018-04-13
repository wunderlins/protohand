#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../lib/stringlib.h"

int main() {
	int i, len;
	char** ar;
	char* str1 = "\"arg 1\" arg2 \"arg 3\"";
	char* str2 = "";
	char* str3 = "123 456";
	char* str4 = "one";
	
	len = split_arg(str1, &ar);
	printf("len: %d, %s\n", len, str1);
	for (i=0; i<len; i++) {
		printf("%d: %s, ", i, ar[i]);
	}
	printf("\n");
	if (len != 3) return 1;
	
	len = split_arg(str2, &ar);
	printf("len: %d, %s\n", len, str2);
	for (i=0; i<len; i++) {
		printf("%d: %s, ", i, ar[i]);
	}
	printf("\n");
	if (len != 0) return 1;
	
	len = split_arg(str3, &ar);
	printf("len: %d, %s\n", len, str3);
	for (i=0; i<len; i++) {
		printf("%d: %s, ", i, ar[i]);
	}
	printf("\n");
	if (len != 2) return 1;
	
	len = split_arg(str4, &ar);
	printf("len: %d, %s\n", len, str4);
	for (i=0; i<len; i++) {
		printf("%d: %s, ", i, ar[i]);
	}
	printf("\n");
	if (len != 1) return 1;
	
	return 0;
}
