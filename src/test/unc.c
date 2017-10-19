#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char * argv[]) {
	// read file from argv[1]
	FILE *f = fopen("\\\\localhost\\c$\\temp\\test.txt", "rb");
	if (f == NULL)
		return 1;
	
	fseek(f, 0, SEEK_END);
	long fsize = ftell(f);
	fseek(f, 0, SEEK_SET);  //same as rewind(f);
	//printf("%ld\n", fsize);
	char *string = (char*) malloc(fsize + 1);
	fread(string, fsize, 1, f);
	string[fsize] = 0;
	fclose(f);
	
	printf("%s\n", string);
	
	return 0;
}