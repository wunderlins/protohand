#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char xor_magic[5] = "0x66";
typedef enum {DECODE, ENCODE} xormode_t;

char *xor(char *string, const char *key, long l) {
	int i = 0;
	char* out = malloc(l);
	size_t length = strlen(key);
	for(i=0; i<l; i++) {
		out[i] = (char) (string[i] ^ key[i % length]);
	}
	return out;
}

int transcode(char* fin, char* fout, char* key) {
	
	xormode_t mode = ENCODE;
	
	// read file from argv[1]
	FILE *f = fopen(fin, "rb");
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
	
	// write file to argv[2]
	f = fopen(fout, "wb");
	if (f == NULL)
		return 2;
	
	if (string[0] == xor_magic[0] && 
	    string[1] == xor_magic[1] && 
		string[2] == xor_magic[2] && 
		string[3] == xor_magic[3]) {
		string += 4;
		fsize -= 4;
		//printf("Decode\n");
		mode = DECODE;
	} else {
		; //printf("Encode\n");
	}
	
	//printf("%ld\n", fsize);
	char *result; // = (char*) malloc(fsize + 1);
	result = xor(string, key, fsize);
	if (mode == ENCODE)
		fwrite(xor_magic, 1, 4, f);
	//printf("%ld\n", fsize);
	fwrite(result, 1, fsize, f);
	fclose(f);
	//fflush(stdout);

	free(result);
	//free(string);

	return 0;
}

#ifdef XOR_MAIN
int main(int argc, char * argv[]) {
	int res = transcode(argv[1], argv[2], argv[3]);
	printf("res: %d\n", res);
	fflush(stdout);
}
#endif