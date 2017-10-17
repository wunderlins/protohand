#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *xor(char *string, const char *key, long l) {
	int i = 0;
	char* out = malloc(l);
	size_t length = strlen(key);
	for(i=0; i<l; i++) {
		out[i] = (char) (string[i] ^ key[i % length]);
	}
	return out;
}

/*
int main(int argc, char **argv)
{
    const char *key = "abc";
    if (argc < 2) {
            fprintf(stderr, "%s: no input\n", argv[0]);
            return EXIT_FAILURE;
    }
    printf("%s\n", xor(xor(argv[1], key), key));
    return EXIT_SUCCESS;
}
*/

int main(int argc, char * argv[]) {
	
	// read file from argv[1]
	FILE *f = fopen(argv[1], "rb");
	fseek(f, 0, SEEK_END);
	long fsize = ftell(f);
	fseek(f, 0, SEEK_SET);  //same as rewind(f);
	printf("%ld\n", fsize);
	char *string = (char*) malloc(fsize + 1);
	fread(string, fsize, 1, f);
	string[fsize] = 0;
	fclose(f);
	
	// write file to argv[2]
	f = fopen(argv[2], "wb");
	char *result; // = (char*) malloc(fsize + 1);
	result = xor(string, argv[3], fsize);
	fwrite(result, 1, fsize, f);
	fclose(f);

	free(result);
	free(string);

	return 0;
}
