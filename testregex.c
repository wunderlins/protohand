#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "lib/regcpp.h"

void usage() {
	printf("testregex.exe </search/replce/> <file>\n");
	printf("\nThis program can be used to test the regex search and replace functionality.\n");
	printf(" - argument 1: a pcre compatible regex and replace pattern. Use $1 for \n");
	printf("               backreferences, make sure to propperly escape special characters\n");
	printf("               of your shell.\n");
	printf(" - argument 2: path to the file to work on\n\n");
	printf("The result will be printed to stdout. If you want it to be in a file\n");
	printf("use the output redirection of your shell, normally\n");
	printf("'testregex.exe /search/replace/ file > outputfile'.\n");
}

int main(int argc, char **argv) {
	
	// check parameters
	if (argc == 2 && (
	    strcmp(argv[1], "-h") == 0 || 
	    strcmp(argv[1], "/h") == 0 || 
	    strcmp(argv[1], "/?") == 0 || 
	    strcmp(argv[1], "--help") == 0)) {
		usage();
		return 0;
	}
	
	if (argc != 3) {
		usage();
		return 1;
	}
	
	// open file
	FILE *f = fopen(argv[2], "rb");
	if (f == NULL) {
		fprintf(stderr, "Failed to open file.\n");
		return 2;
	}
	
	fseek(f, 0, SEEK_END);
	long fsize = ftell(f);
	fseek(f, 0, SEEK_SET);  //same as rewind(f);

	char *string = (char*) malloc(fsize + 1);
	fread(string, fsize, 1, f);
	fclose(f);

	string[fsize] = 0;
	
	const char* result;
	//const char* reg = "pre/[0-9]+/NNNN/m";
	
	printf("regex: %s\n", argv[1]);
	result = regreplace(argv[1], string);
	
	if (result == NULL) {
		printf("Failed to execute regex: %d\n", regerrno);
	}
	
	printf("%s\n", result);
	
	return 0;
}