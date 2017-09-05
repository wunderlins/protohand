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

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "ini.h"
#include "urldecode2.h"

// maximum input lengths 
// from stdin, prevent buffer overflows
#define STDIN_MAX 1024
// for paths
#define MAX_CWD_LENGTH 1024

// Error codes, used as return codes
#define OK            0
#define NO_INPUT      1
#define TOO_LONG      2
#define NO_SCHEME     3
#define NO_AUTHORITY  4
#define NO_CURRENTDIR 5
#define UNKNOWN_INI_KEY 6

#define INI_FILE_NAME "protohand.ini"

// debug levels, 0=off, 1=on
#ifndef DEBUG
	#define DEBUG 0
#endif

// can hold one ini file entry
typedef struct {
	const char* section; // the section we are searchin for
	const char* default_path;
	const char* allowed_params;
	const char* path_params;
	const char* exe;
	int found; // 1 if the section was found. initialize it to 0 otherwise
} configuration;

char separator() {
#ifdef _WIN32
	return '\\';
#else
	return '/';
#endif
}

/**
 * callback function for ini file parser
 */
static int dumper(void* user, const char* section, const char* name,
                  const char* value) {
	//static char prev_section[50] = "";
	configuration* pconfig = (configuration*)user;

	#define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
	if (strcmp(pconfig->section, section) == 0) {
		printf("%s => %s: %s [%s]\n", section, name, value, pconfig->section);

		if (MATCH(section, "default_path")) {
			pconfig->default_path = strdup(value);
			pconfig->found = 1;
		} else if (MATCH(section, "allowed_params")) {
			pconfig->allowed_params = strdup(value);
			pconfig->found = 1;
		} else if (MATCH(section, "path_params")) {
			pconfig->path_params = strdup(value);
			pconfig->found = 1;
		} else if (MATCH(section, "exe")) {
			pconfig->exe = strdup(value);
			pconfig->found = 1;
		} else {
			return 0;
		}
	}
	// printf("search section: %s\n", (const char *) pconfig->section);

	return 1;
}

void usage(void);

int main(int argc, char** argv) {
	const char sep = separator();
	
	// initialize the config 
	int error;
	const char* empty = "";
	configuration config;
	config.section        = empty;
	config.found          = 0;
	config.default_path   = empty;
	config.allowed_params = empty;
	config.path_params    = empty;
	config.exe            = empty;
	
	// url parts buffers
	char buff[STDIN_MAX], scheme[STDIN_MAX], authority[STDIN_MAX], path[STDIN_MAX], query[STDIN_MAX], reminder[STDIN_MAX];
	size_t sz = sizeof(buff);
	int ch, extra;
	
	// set them all to empty strings
	buff[0] = '\0';
	scheme[0] = '\0';
	authority[0] = '\0';
	path[0] = '\0';
	query[0]  = '\0';
	reminder[0] = '\0';
	
	// find cwd and set ini path
	char cwd[MAX_CWD_LENGTH];
	if (getcwd(cwd, sizeof(cwd)) == NULL) {
		perror("getcwd() error");
		return NO_CURRENTDIR;
	}
	int l = strlen(cwd);
	cwd[l] = sep;
	cwd[l+1] = '\0';
	
	char ini_file[MAX_CWD_LENGTH];
	strcpy(ini_file, cwd);
	strcat(ini_file, INI_FILE_NAME);
	
	#if DEBUG == 1
	fprintf(stdout, "Current working dir: %s\n", cwd);
	fprintf(stdout, "Current ini file:    %s\n", ini_file);
	#endif
	
	// read stdin
	if (feof(stdin)) {
		// read data from stdin. no data? return error.
		if (fgets (buff, sz, stdin) == NULL) {
			perror("no input on stdin");
			return NO_INPUT;
		}
		
		// If it was too long, there'll be no newline. In that case, we flush
		// to end of line so that excess doesn't affect the next call.
		if (buff[strlen(buff)-1] != '\n') {
			extra = 0;
			while (((ch = getchar()) != '\n') && (ch != EOF))
				extra = 1;
			
			if (extra == 1) {
				perror("Too much data on stdin");
				return TOO_LONG;
			}
		}
	} else if(argc == 2) {
		// no data from stdin, try to read argv[1]
		int ln = strlen(argv[1]);
		//printf("%d\n", ln);
		strncpy(buff, argv[1], ln);
		buff[ln] = '\0';
	} else {
		perror("argument 1 with uri missing");
		usage();
		return NO_INPUT;
	}
	
	// remove trailing newline
	int i;
	for(i=0; i<strlen(buff)+1; i++) {
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
	printf("stdin: %d, buff: %d\n", (int) strlen(argv[1]), (int) strlen(buff));
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
	if (found == 0) {
		perror("uri parse error: no scheme found");
		return NO_SCHEME;
	}
	
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
	if (strlen(reminder) == 0) {
		perror("uri parse error: no authority found");
		return NO_AUTHORITY;
	}
	
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
	if (found_slash == 0 || found_questionmark == 0) {
		perror("uri parse error: no othority found");
		return NO_AUTHORITY;
	}
	
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
	
	// urldecode params
	char *query_escaped = malloc(strlen(query)+1);
	urldecode2(query_escaped, query);
	
	// display parsed uri
	#if DEBUG == 1
	printf("scheme    (%d): '%s'\n", found, scheme);
	printf("authority (%d): '%s'\n", (int) strlen(authority), authority);
	printf("path      (%d): '%s'\n", found_slash, path);
	printf("query     (%d): '%s'\n", found_questionmark, query_escaped);
	#endif
	
	// find the appropriate config in the ini file
	// try to read ini file
	char section[STDIN_MAX] = "";
	strcpy(section, authority);
	//strcat(section, "/");
	strcat(section, path);
	config.section = section;
	error = ini_parse(ini_file, dumper, &config);
	
	if (error == 0) {
		perror("unknown ini section or key");
		return UNKNOWN_INI_KEY;
	}
	
	#if DEBUG == 1
	printf("section:        %s\n", config.section);
	printf("exe:            %s\n", config.exe);
	printf("default_path:   %s\n", (config.default_path == empty) ? "(unset)" :  config.default_path);
	printf("allowed_params: %s\n", (config.allowed_params == empty) ? "(unset)" :  config.allowed_params);
	printf("path_params:    %s\n", (config.path_params == empty) ? "(unset)" :  config.path_params);
	#endif
	
	return OK;
}

void usage(void) {
printf("Usage: protohand.exe \"scheme:[//]authority[/path[?query]]\"\n"
"\n"
"scheme:    the protocol part of your uri\n"
"authority: then`host` or exe part of your uri\n"
"path:      path contains the uri path or application handler\n"
"query:     uri query part (must be url encoded) which translates to command line parameters\n"
"\n"
"On windows you need to register your protocol handler in the registry.\n"
"Replace <yourapp> with the protocol name, and <path> with the absolute path \n"
"to your executable. Example reg file:\n"
"\n"
"==[REG FILE] ==========================\n"
"Windows Registry Editor Version 5.00\n"
"\n"
"[HKEY_CLASSES_ROOT\\yourapp]\n"
"@=\"URL: Your Protocol\"\n"
"\"URL Protocol\"=\"\"\n"
"\n"
"[HKEY_CLASSES_ROOT\\yourapp\\shell]\n"
"[HKEY_CLASSES_ROOT\\yourapp\\shell\\open]\n"
"[HKEY_CLASSES_ROOT\\yourapp\\shell\\open\\command]\n"
"@=\"\\\"C:\\\\path\\\\protohand.exe\\\" \\\"%%1\\\"\"\n"
"==[EOF] ==============================\n"
"\n"
"Exit Codes:\n"
"\n"
"0 OK\n"
"1 NO_INPUT - empty string on stdin\n"
"2 TOO_LONG - too long string on stdin, max(%d)\n"
"3 NO_SCHEME - url parser did not find a scheme\n"
"4 NO_AUTHORITY - url parser did not find authority\n"
"5 NO_CURRENTDIR - cwd error, could not find current working dir\n"
"6 UNKNOWN_INI_KEY - parser error, unrecognized section or key in ini file\n"
"\n"
, STDIN_MAX);
}
