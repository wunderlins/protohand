#include "getexe.h"

int getexe(char* cmd, char* exe) {
	exe = (char*) malloc(sizeof(char) * (strlen(cmd)+1));
	
	printf("%s\n", cmd);
	char* trimmed;
	trim(cmd);
	return 0;
	
	char quote = isquoted(trimmed);
	
	printf("%c, '%s'\n", quote, trimmed);
	
	return 0;
}

#ifdef GETEXE_MAIN
int main(int argc, char** argv) {
	int ret = 0;
	char* exe = "";
	
	char* cmd1 = " \"C:/Users/wus/Projects/protohand/src/testcmd.exe\"";
	char* cmd2 = " \"C:/Users/wus/Projects/protohand/src/testcmd.exe\"";
	char* cmd3 = "\"C:\\Program Files (x86)\\Internet Explorer\\iexplore.exe\"  \"http://www.google.com\"  ";
	
	getexe(cmd1, exe);
	//getexe(cmd2, exe);
	//getexe(cmd3, exe);
	
	return ret;
}

#endif