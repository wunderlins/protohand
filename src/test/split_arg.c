#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../lib/stringlib.h"

int split_arg(char* str, char*** args) {
	size_t i = 0;
	size_t len = strlen(str);
	int open = 0;
	char buffer[2048] = {0};
	int buff_len = 0;
	int arg_len = 0;
	*args = malloc(sizeof(*args)*1024);
	
	for (i=0; i<len; i++) {
		if (str[i] == '"') {
			open = (open) ? 0 : 1;
			continue;
		}
		
		if (str[i] == ' ' && !open) {
			printf("%s\n", buffer);
			(*args)[arg_len] = (char*) malloc(sizeof(char)*(strlen(buffer)+1));
			char *p = &(buffer)[0];
			cmdunquote(&p);
			strcpy((*args)[arg_len], buffer);
			arg_len++;
			//args++;
			buffer[0] = 0;
			buff_len = 0;
			continue;
		}
		
		buffer[buff_len++] = str[i];
		buffer[buff_len] = 0;
		
		//printf("%c", str[i]);
	}
	
	if(str[i] == 0 && i) {
		(*args)[arg_len] = (char*) malloc(sizeof(char)*(strlen(buffer)+1));
		char *p = &(buffer)[0];
		cmdunquote(&p);
		strcpy((*args)[arg_len], buffer);
		arg_len++;
		printf("%s\n", buffer);
	}
	
	return arg_len;
}

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
		printf("%d: %s\n", i, ar[i]);
	}
	
	len = split_arg(str2, &ar);
	printf("len: %d, %s\n", len, str2);
	for (i=0; i<len; i++) {
		printf("%d: %s\n", i, ar[i]);
	}
	
	len = split_arg(str3, &ar);
	printf("len: %d, %s\n", len, str3);
	for (i=0; i<len; i++) {
		printf("%d: %s\n", i, ar[i]);
	}
	
	len = split_arg(str4, &ar);
	printf("len: %d, %s\n", len, str4);
	for (i=0; i<len; i++) {
		printf("%d: %s\n", i, ar[i]);
	}
	
	return 0;
}
