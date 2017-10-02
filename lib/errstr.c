#include "errstr.h"
char* errstr[255] = { NULL };

/* Make this header file easier to include in C++ code */
#ifdef __cplusplus
extern "C" {
#endif

void define_error_messages() {
	errstr[0] = "Ok";
	errstr[1] = "Argument 1 missing.";
	errstr[2] = "Failed to find current directory. getcwd() or realpath() failed to resolve the app directory.";
	errstr[3] = "Failed to create ph.ini, permission denied.";
	errstr[4] = "Failed to create ph.ini, unable to write to file.";
	errstr[5] = "Failed to parse ini file.";
	errstr[6] = "Wrong path or program is not executable.";
	errstr[7] = "No inisection found.";
	errstr[8] = "While generating the commandline string, too many arguments were passed in via query and/or configuration. The limit is: MAX_PARAMS.";

	errstr[9] = "Failed to open file for replacement.";
	errstr[10] = "Failed to parse regular expression.";
	
	errstr[128] = "Failed to parse URI, protocol missing.";
	errstr[129] = "Failed to parse URI, authority missing.";
	errstr[130] = "Failed to parse URI, error in query string.";
	errstr[131] = "Failed to parse URI, error in fragment.";
	//errstr[] = "";
}

#ifdef __cplusplus
}
#endif