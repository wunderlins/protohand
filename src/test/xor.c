#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "xor.h"

const char xor_magic[5] = "0x66";

char *xor(char *string, const char *key, long l) {
	int i = 0;
	char* out = malloc(l);
	size_t length = strlen(key);
	for(i=0; i<l; i++) {
		out[i] = (char) (string[i] ^ key[i % length]);
	}
	return out;
}

int transcode_file(char* fin, char* fout, char* key) {
	
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

int transcode_str(char* strin, long* fsize, char** strout, char* key) {
	
	xormode_t mode = ENCODE;
	long outlen;
	
	if (strin[0] == xor_magic[0] && 
	    strin[1] == xor_magic[1] && 
		strin[2] == xor_magic[2] && 
		strin[3] == xor_magic[3]) {
		strin += 4;
		outlen = *fsize-4;
		mode = DECODE;
		//printf("Decode\n");
	} else {
		outlen = *fsize+4;
		//printf("Encode\n");
	}
	
	char *result = (char*) malloc(outlen);
	if (mode == ENCODE) {
		memcpy(result, xor_magic, 4);
		memcpy(result+4, xor(strin, key, *fsize), *fsize);
	} else 
		memcpy(result, xor(strin, key, *fsize), *fsize);
	
	/*
	long i;
	for(i=0; i<outlen; i++) {
		if (mode == ENCODE && (result[i] == '\r' || result[i] == '\n'))
			result[i] = 0;
		if (mode == DECODE && result[i] == '\0')
			result[i] = '\n';
	}
	*/
	
	*strout = result;
	*fsize = outlen;
	
	return 0;
}
#ifdef XOR_MAIN
int main(int argc, char * argv[]) {

	FILE *f = fopen(argv[1], "rb");
	if (f == NULL)
		return 1;
	fseek(f, 0, SEEK_END);
	long fsize = ftell(f);
	fseek(f, 0, SEEK_SET);  //same as rewind(f);
	//printf("%ld\n", fsize);
	char *string = (char*) malloc(fsize);
	fread(string, fsize, 1, f);
	//string[fsize] = 0;
	fclose(f);
	
	char *result;
	int res = transcode_str(string, &fsize, &result, argv[3]);
	//printf("%s", result);
	//fwrite(result, 1, fsize, stdout);
	//printf("\n");
	fflush(stdout);
	
	f = fopen(argv[2], "wb");
	fwrite(result, 1, fsize, f);
	fclose(f);
	
	//printf("%s\n", xor(xor(string, "123", fsize), "123", fsize));
	
	return 0;
}
#endif