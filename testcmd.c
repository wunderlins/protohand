#include <stdio.h>

int main(int argc, char** argv, char **envp) {
	int i=0;
	for(; i<argc; i++)
		printf("%s ", argv[i]);
	return 0;
}