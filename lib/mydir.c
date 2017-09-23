#include "mydir.h"

void fwdslash(char* path) {
	int i;
	int l = strlen(path);
	for (i=0; i<l; i++)
		if (path[i] == '\\') path[i] = '/';
	return;
}

int exedir(char *argv0, char *exedir) {
	int i;
	char tmp[MAX_CWD_LENGTH];
	char cleaned[MAX_CWD_LENGTH] = "";
	
	// get the current working directory
	char cwd[MAX_CWD_LENGTH];
	if (getcwd(cwd, sizeof(cwd)) == NULL) {
		perror("getcwd() error");
		return 1;
	}
	
	int l = strlen(cwd);
	cwd[l] = '/';
	cwd[l+1] = '\0';
	
	// change all backslashes to forward slashes
	fwdslash(cwd);
	fwdslash(argv0);
	
	// concatenate cwd and argv0
	if (argv0[1] == ':' || argv0[0] == '/') {
		strcpy(tmp, argv0);
	} else {
		strcpy(tmp, cwd);
		strcat(tmp, argv0);
	}
	
	// remove the filename from tmp
	for(i=l; i>0; i--) {
		if (tmp[i] == '/') {
			tmp[i+1] = '\0';
			break;
		}
	}
	
	// clean the path, remove all . and ..
	errno = 0;
	//printf("%s\n", tmp);
	realpath(tmp, cleaned);
	if (errno != 0) {
		perror("realpath failed");
		return 2;
	}
	
	// rewind from the end until the first '/' is found
	fwdslash(cleaned);
	l = strlen(cleaned);
	
	strcpy(exedir, cleaned);
	
	//printf("cwd:   %s\nargv0: %s\nres:   %s\nclean: %s\n", cwd, argv0, res, cleaned);
	
	return 0;
}

#ifdef MYDIR_MAIN
int main(int argc, char* argv[]) {
	char *dir = malloc(sizeof(char*) * (MAX_CWD_LENGTH+1));
	int ret = exedir(argv[0], dir);
	printf("exedir: %s\n", dir);
	return ret;
}
#endif