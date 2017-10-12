#include <stdio.h>
#include "lib/mydir.h"

int main(int argc, char** argv, char **envp) {
	FILE *logfile;
	char *dir = malloc(sizeof(char*) * (MAX_CWD_LENGTH+1));
	exedir(argv[0], dir);

	char log_file[MAX_CWD_LENGTH];
	strcpy(log_file, dir);
	strcat(log_file, "testcmd.log");
	logfile = fopen(log_file, "ab+");

	int i=0;
	for(; i<argc; i++) {
		fprintf(logfile, "%s ", argv[i]);
		fprintf(stdout, "%s ", argv[i]);
	}
	fprintf(logfile, "\n");
	fprintf(stdout, "\n");
	fclose(logfile);
	
	return 0;
}