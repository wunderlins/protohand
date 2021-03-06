#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char isquoted(char* str) {
	if (str[0] == '"') {
		if (str[strlen(str)-1] == '"')
			return '"';
		return 0;
	}
	
	if (str[0] == '\'') {
		if (str[strlen(str)-1] == '\'')
			return '\'';
		return 0;
	}
	return 0;
}

int cmdquote(char** str) {
	char quote = '"';
	int l = strlen(*str);
	int i = 0;
	int p = 1;
	int add = 0;
	char *result = malloc(sizeof(char*) * (l+3));
	
	result[0] = quote;
	for(i=0; i<l; i++) {
		if (str[0][i] == quote) {
			result[p++] = quote;
			add++;
			result = realloc(result, 5 * sizeof(char*) * (l+3+add));
			if (result == NULL)
				return 1;
		}
		result[p++] = str[0][i];
	}
	result[p] = quote;
	result[p+1] = '\0';
	
	//printf("%s\n", result);
	*str = result;
	
	return 0;
}

int cmdunquote(char** str) {
	int l = strlen(*str);
	int i = 0;
	for(i=0;i<l-1; i++) {
		str[0][i] = str[0][i+1];
	}
	str[0][l-2] = '\0';
	return 0;
}

#ifdef QUOTE_MAIN
int main(int argc, char* argv[]) {
	
	char *quoted1 = "\"asd\"";
	char *quoted2 = "'quertz'";
	char *quoted3 = "\"asd";
	char *quoted4 = "'quertz";
	char *quoted5 = "que\"rtz";
	char *quoted6 = "que'rtz";
	
	printf("%d, %s\n", isquoted(quoted1), quoted1);
	printf("%d, %s\n", isquoted(quoted2), quoted2);
	printf("%d, %s\n", isquoted(quoted3), quoted3);
	printf("%d, %s\n", isquoted(quoted4), quoted4);
	
	cmdquote(&quoted5);
	printf("%s\n", quoted5);
	cmdquote(&quoted6);
	printf("%s\n", quoted6);
	cmdunquote(&quoted6);
	printf("%s\n", quoted6);
	
	return 0;
}
#endif