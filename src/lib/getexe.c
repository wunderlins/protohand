#include "getexe.h"

int getexe(char* cmd, char* exe) {
	exe = (char*) malloc(sizeof(char) * (strlen(cmd)+1));
	strcpy(exe, cmd);
	
	exe = trim(exe);
	char quote = isquoted(exe);
	
	// skip first quote char
	if (quote != 0) exe++; else quote = ' ';
	int l = strlen(exe);
	int i = 0;
	for (i=0; i<l; i++) {
		if (exe[i] == quote) {
			exe[i] = 0;
			break;
		}
	}
	
	//printf("%c, '%s'\n", quote, exe);
	
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
	
	getexe(cmd1, exe);
	getexe(cmd2, exe);
	getexe(cmd3, exe);
	getexe(cmd4, exe);
	
	return ret;
}

#endif