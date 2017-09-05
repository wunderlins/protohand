/**
 * Read string from stdinput nd parse uri parts [authority,path,query].
 * for a detailed URI spec see: https://tools.ietf.org/html/rfc3986
 * (See section 3 for a quick start about schemes).
 *
 * `scheme` is used by the os to invoke this program.
 * The program extracts then `authority` and maps it to an executable (external mapping).
 * FIXME: the `path`part can be useful but it's unclear for what yet.
 * the `query` part shall hold all program parameters. PAthis in the parameters must be checked and sanitized. The query string must be a properly urlencoded associtative array of program parameters in the form of:
 *   windows: ?/a&/b=1 which translates into "/a /b=1" command line parameters
 *   *nix:    ?-y&--long-param=fubar which translates into: "-y --long-param=fubar"
 *   attributes: ?param1&param2 which translates into "param1 param2"
 * all schemes above can be mxed since it is up to the called program to interpret the commandline arguments and therefore the called program's specs must be met.
 *
 * 2017, Simon Wunderlin <swunderlin@gmail.com>
 */

#include <stdio.h>
#include <string.h>

// maximum input length from stdin, prevent buffer overflows
#define STDIN_MAX 1024

// Error codes, used as return codes
#define OK       0
#define NO_INPUT 1
#define TOO_LONG 2
#define NO_SCHEME 3
#define NO_AUTHORITY 4

// debug levels, 0=off, 1=on
#define DEBUG 1

int main(int argc, char** argv) {
	
	char buff[STDIN_MAX], scheme[STDIN_MAX], authority[STDIN_MAX], path[STDIN_MAX], query[STDIN_MAX], reminder[STDIN_MAX];
	size_t sz = sizeof(buff);
	int ch, extra;
	
	buff[0] = '\0';
	scheme[0] = '\0';
	authority[0] = '\0';
	path[0] = '\0';
	query[0]  = '\0';
	reminder[0] = '\0';
	
	if (feof(stdin)) {
		// read data from stdin. no data? return error.
		if (fgets (buff, sz, stdin) == NULL)
			return NO_INPUT;
		
		// If it was too long, there'll be no newline. In that case, we flush
		// to end of line so that excess doesn't affect the next call.
		if (buff[strlen(buff)-1] != '\n') {
			extra = 0;
			while (((ch = getchar()) != '\n') && (ch != EOF))
				extra = 1;
			
			if (extra == 1)
				return TOO_LONG;
		}
	} else if(argc == 2) {
		// no data from stdin, try to read argv[1]
		strncpy(buff, argv[1], strlen(argv[1]));
	} else {
		return NO_INPUT;
	}
	
	// remove trailing newline
	for(int i=0; i<strlen(buff); i++) {
		if (buff[i] == '\n' || buff[i] == '\r') {
			buff[i] = '\0';
			break;
		}
	}
	
	#if DEBUG == 1
	// write debug log
	FILE* logfile;
	logfile = fopen("protohand.log", "wb+");
	fwrite(buff, strlen(buff), 1, logfile);
	fclose(logfile);
	#endif
	
	
	#if DEBUG == 1
	printf("===========================\n");
	printf("stdin: %s\n", buff);
	#endif
	
	// start parsing the string, find the scheme, check for ':'
	int found = 0;
	int counter = 0;
	for(; counter<STDIN_MAX && found == 0; counter++) {
		if (buff[counter] == ':') {
			found = counter;
			break;
		}
	}
	
	// no scheme found, exit with error
	if (found == 0)
		return NO_SCHEME;
	
	// copy scheme to scheme buffer
	strncpy(scheme, buff, found);
	scheme[found] = '\0';
	
	// copy rest to reminder
	strncpy(reminder, buff+found+1, strlen(buff)-found);
	reminder[strlen(buff)-found-1] = '\0';
	
	// if the first characters of reminder are '/[/]', remove them
	int len = strlen(reminder);
	if (reminder[0] == '/' && reminder[1] == '/')
		strncpy(reminder, reminder+2, len);
	
	// if there is nothing left in the reminder, we got an malformed uri,
	// missing an authority
	if (strlen(reminder) == 0)
		return NO_AUTHORITY;
	
	// find the first occourance of '/' or '?'
	int found_slash = -1;
	int found_questionmark = -1;
	counter = 0;
	
	for(; counter<strlen(reminder); counter++) {
		if (reminder[counter] == '/') 
			found_slash = counter;
		if (reminder[counter] == '?') 
			found_questionmark = counter;
		if (found_questionmark > -1 && found_slash > -1)
			break;
	}
	
	// if questionmark or path is at position 0 of reminder, we have no authority
	if (found_slash == 0 || found_questionmark == 0)
		return NO_AUTHORITY;
	
	// if we haven't found a questionmark and have not found a slash,
	// the reminder equals to authority
	if (found_slash == -1 && found_questionmark == -1) {
		strncpy(authority, reminder, strlen(reminder)+1);
	}
	
	// authority and path
	if (found_slash > 0 && found_questionmark == -1) {
		strncpy(authority, reminder, strlen(reminder));
		authority[found_slash] = '\0';
		strncpy(path, reminder+found_slash, strlen(reminder)+1-found_slash);
	}
	
	// authority and query
	if (found_slash == -1 && found_questionmark > 0) {
		strncpy(authority, reminder, strlen(reminder));
		authority[found_questionmark] = '\0';
		strncpy(query, reminder+found_questionmark+1, strlen(reminder)+1-found_questionmark);
	}
	
	// authority and path and query
	if (found_slash > 0 && found_questionmark > 0) {
		strncpy(authority, reminder, strlen(reminder));
		authority[found_slash] = '\0';
		strncpy(path, reminder+found_slash, found_questionmark-found_slash);
		strncpy(query, reminder+found_questionmark+1, strlen(reminder)+1-found_questionmark);
	}
	
	#if DEBUG == 1
	printf("scheme    (%d): '%s'\n", found, scheme);
	printf("authority (%d): '%s'\n", strlen(authority), authority);
	printf("path      (%d): '%s'\n", found_slash, path);
	printf("query     (%d): '%s'\n", found_questionmark, query);
	#endif
	
	return OK;
}