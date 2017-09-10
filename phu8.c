#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char** argv, char **envp) {
	
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <uri>\n", argv[0]);
		return 1;
	}
	
	char *uri = malloc(sizeof(argv[1])+2);
	strncpy(uri, argv[1], strlen(argv[1])+1);
	printf("%s\n", argv[1]);
	return 0;
}