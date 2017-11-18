#include "errstr.h"

/* Make this header file easier to include in C++ code */
#ifdef __cplusplus
extern "C" {
#endif

char* errstr[255] = { NULL };

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
	errstr[11] = "Could not allocate enough memmory to read the file into RAM for replacement.";
	errstr[12] = "replace_file set but replace_regex empty or missing.";
	
	errstr[13] = "Failed to find ini Section.";
	errstr[14] = "Failed to expand environment variable.";
	errstr[15] = "Missing `cmd` directive in ini file.";

	errstr[16] = "Failed to delete log file.";
	errstr[17] = "Failed to rename log file.";
	errstr[18] = "Failed to find userdir (APPDATA/HOME) in environment variables.";
	errstr[19] = "Failed to open config file.";
	errstr[20] = "Failed to open log file.";
	errstr[21] = "Failed to download config file.";
	
	errstr[33] = "No equal sign found in expression expansion.";
	errstr[34] = "No colon found in expression expansion.";
	errstr[35] = "Failed to dynamically allocate memmory while expanding variables (malloc).";
	errstr[36] = "Ennvironment variable not found while trying to expand.";
	errstr[37] = "Query parameter not found while expaning.";
	errstr[38] = "Failed to dynamically allocate memmory while expanding variables (realloc).";
	
	errstr[128] = "Failed to parse URI, protocol missing.";
	errstr[129] = "Failed to parse URI, authority missing.";
	errstr[130] = "Failed to parse URI, error in query string.";
	errstr[131] = "Failed to parse URI, error in fragment.";
	//errstr[] = "";
}

#ifdef __cplusplus
}
#endif