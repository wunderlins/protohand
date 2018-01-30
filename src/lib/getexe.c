#include "getexe.h"

int getexe(char* cmd, char** exe) {
	char* tmp = (char*) malloc(sizeof(char) * (strlen(cmd)+1));
	strcpy(tmp, cmd);
	
	tmp = trim(tmp);
	char quote = isquoted(tmp);
	
	// skip first quote char
	if (quote != 0) tmp++; else quote = ' ';
	int l = strlen(tmp);
	int i = 0;
	//printf("%d, %c, %s\n", l, quote, tmp);
	//return 0;
	for (i=0; i<l; i++) {
		if (tmp[i] == quote) {
			tmp[i] = 0;
			break;
		}
		if (tmp[i] == 0)
			break;
	}
	
	*exe = (char*) malloc(sizeof(char) * (strlen(tmp)+1));
	strcpy(*exe, tmp);
	printf("%c, '%s'\n", quote, *exe);
	
	return 0;
}

#ifdef GETEXE_MAIN
int main(int argc, char** argv) {
	int ret = 0;
	char* exe = "";
	
	char* cmd1 = " \"C:/Users/wus/Projects/protohand/src/testcmd.exe\"";
	char* cmd2 = " \"C:/Users/wus/Projects/protohand/src/testcmd.exe\"";
	char* cmd3 = "\"C:\\Program Files (x86)\\Internet Explorer\\iexplore.exe\"  \"http://www.google.com\"  ";
	char* cmd4 = "123";
	
	getexe(cmd1, &exe);
	getexe(cmd2, &exe);
	getexe(cmd3, &exe);
	getexe(cmd4, &exe);
	
	return ret;
}

#endif