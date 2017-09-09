/**
 * Read string from stdinput and parse uri parts [authority,path,query].
 * for a detailed URI spec see: https://tools.ietf.org/html/rfc3986
 * (See section 3 for a quick start about schemes).
 *
 * `scheme` is used by the os to invoke this program.
 * The program extracts then `authority` and maps it to an executable (external mapping).
 * the `query` part shall hold all program parameters. PAthis in the parameters must be checked and sanitized. The query string must be a properly urlencoded associtative array of program parameters in the form of:
 *   windows: ?/a&/b=1 which translates into "/a /b=1" command line parameters
 *   *nix:    ?-y&--long-param=fubar which translates into: "-y --long-param=fubar"
 *   attributes: ?param1&param2 which translates into "param1 param2"
 * all schemes above can be mxed since it is up to the called program to interpret the commandline arguments and therefore the called program's specs must be met.
 *
 
  protohand://example/over/there?-a=1&-b=2#nose
        \_/   \______/\________/ \_______/ \__/
         |        |         |           |    |____
      scheme  authority     path        query   fragment
         |     ___|______   ____|____    __|____    |_
        / \   /          \ /         \  /       \     |
    [protocol]  [profile]  example.exe  -a=1 -b=2    unused
  
[protocol] is used by the Operating system and has no effect in this program.
[profile] is used to define different actions for one executable.

 * 2017, Simon Wunderlin <swunderlin@gmail.com>
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <process.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>
#include "ini.h"
#include "example_ini.h"
#include "README.h"
#include "stringlib.h"
#include "realpath.h"

#ifndef LOG_TO_FILE
	#define LOG_TO_FILE 0
#endif

// debug levels, 0=off, 1=on
#ifndef DEBUG
	#define DEBUG 0
#endif

// maximum input lengths 
// from stdin, prevent buffer overflows
#ifndef STDIN_MAX
	#define STDIN_MAX 1024
#endif

// for paths
#ifndef MAX_CWD_LENGTH
	#define MAX_CWD_LENGTH 1024
#endif

// if there are more unvalidated parameters passed in via URI query, the 
// program will give up. We assume that this is certainly a bogous request
#ifndef MAX_UNVALIDATED_PARAMETERS
	#define MAX_UNVALIDATED_PARAMETERS 5
#endif

// debug outut macro for str_array
#if DEBUG > 1
	#define \
		str_array_debug(name, obj) ({ \
			fprintf(logfile, "%s count: %d\n", name, obj.length); \
			for (i = 0; i < obj.length; ++i) \
				fprintf(logfile, "     param: %s\n", obj.items[i]); \
		});
	
#elif DEBUG == 1
	#define \
		str_array_debug(name, obj) ({ \
		fprintf(logfile, "%s count: %d\n", name, obj.length); });
#else 
	#define str_array_debug(name, obj) ;
#endif

// Error codes, used as return codes
#define OK              0
#define NO_INPUT        1
#define TOO_LONG        2
#define NO_SCHEME       3
#define NO_AUTHORITY    4
#define NO_CURRENTDIR   5
#define UNKNOWN_INI_KEY 6
#define UNALLOWED_CHAR_IN_AUTHORITY 11
#define UNALLOWED_CHAR_IN_PATH 12
#define PARAM_SPLIT_ERROR 13
#define UNALLOWED_PARAM 14
#define TOO_MANY_PARAMETERS 15
#define PATH_NOT_ALLOWED 16
#define PROGRAM_IS_NOT_EXECUTABLE 17
#define INI_PERMISSION_DENIED 18
#define INI_CREATION_FAILED 19
#define INVALID_PATH 20

#define INI_FILE_NAME "protohand.ini"

// can hold one ini file entry
typedef struct {
	const char* section; // the section we are searchin for
	const char* default_path;
	const char* allowed_params;
	const char* path_params;
	const char* params_prepend;
	const char* params_append;
	const char* exe;
	int found; // 1 if the section was found. initialize it to 0 otherwise
} configuration;

// characters allowed in host names and paths
const char* allowed = "acbcdefghijklmnopqrstuvwxyz"
                      "ACBCDEFGHIJKLMNOPQRSTUVWXYZ"
                      "0123456789"
                      "-._~"; // RFC3986, 2.3.  Unreserved Characters

char separator() {
#ifdef _WIN32
	return '\\';
#else
	return '/';
#endif
}

void printerr(char* errstr) {
	fprintf(stderr, errstr);
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
		#if DEBUG > 1
		fprintf(logfile, "%s => %s: %s [%s]\n", section, name, value, pconfig->section);
		#endif

		if (MATCH(section, "default_path")) {
			pconfig->default_path = strdup(value);
			pconfig->found = 1;
		} else if (MATCH(section, "allowed_params")) {
			pconfig->allowed_params = strdup(value);
			pconfig->found = 1;
		} else if (MATCH(section, "path_params")) {
			pconfig->path_params = strdup(value);
			pconfig->found = 1;
		} else if (MATCH(section, "params_prepend")) {
			pconfig->params_prepend = strdup(value);
			pconfig->found = 1;
		} else if (MATCH(section, "params_append")) {
			pconfig->params_append = strdup(value);
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

/**
 * Display usage
 */
void usage(void) {
	printf(usage_str);
}

/**
 * Create an example ini file
 */
int create_ini(char* ini_file) {
	fprintf(stderr, "Failed to open the ini file, trying to create it:\n");
	fprintf(stderr, "%s\n", ini_file);
	
	FILE* fpinifile;
	fpinifile = fopen(ini_file, "wb+");
	
	if (fpinifile == NULL) {
		if (errno == EACCES) {
			printerr("Permission denied!\n");
			#if DEBUG > 0
			fprintf(logfile, "Failed to created ini file '%s'. "
							 "Permission denied.\n", ini_file);
			#endif
			return INI_PERMISSION_DENIED;
		}
		
		#if DEBUG > 0
		fprintf(logfile, "Failed to created ini file '%s'. "
						 "Error: %d: %s.\n", ini_file, errno, 
						 strerror(errno));
		#endif
		perror("Failed to create ini file.");
		return INI_CREATION_FAILED;
	}
	
	// write default ini file 
	//printf("length: %d\n", sizeof(ini_str));
	fwrite(ini_str , sizeof(char), strlen(ini_str), fpinifile);
	fclose(fpinifile);
	
	// show user that the ini file was created and quit
	printf("Example ini file has been created. You should review and "
		   "change it to your needs. Aborting.");
	return OK;
}

extern char **environ;
int main(int argc, char** argv, char **envp) {
	environ = envp;
	int i;
	const char sep = separator();
	
	// initialize the config 
	//int error;
	const char* empty = "";
	configuration config;
	config.section        = empty;
	config.found          = 0;
	config.default_path   = empty;
	config.allowed_params = empty;
	config.path_params    = empty;
	config.exe            = empty;
	config.params_append  = empty;
	config.params_prepend = empty;
	
	// url parts buffers
	char buff[STDIN_MAX], scheme[STDIN_MAX], authority[STDIN_MAX], path[STDIN_MAX], query[STDIN_MAX], reminder[STDIN_MAX];
	
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
	
	// open log file
	#if DEBUG > 0
	// write debug log
		#if LOG_TO_FILE == 1
		char log_file[MAX_CWD_LENGTH];
		strcpy(log_file, cwd);
		strcat(log_file, "protohand.log");
		logfile = fopen(log_file, "wb+");
		fwrite(buff, strlen(buff), 1, logfile);
		#else
			logfile = stdout;
		#endif
	#endif
	
	char ini_file[MAX_CWD_LENGTH];
	strcpy(ini_file, cwd);
	strcat(ini_file, INI_FILE_NAME);
	
	// check if the ini file exists. if no try to create it.
	if( access(ini_file, F_OK) == -1 ) {
		// file doesn't exist, create it
		int ret = create_ini(ini_file);
		
		// something went wong? abolt!
		if (ret != OK)
			return ret;
	}
	
	#if DEBUG > 0
	fprintf(logfile, "Current working dir: %s\n", cwd);
	fprintf(logfile, "Current ini file:    %s\n", ini_file);
	#endif
	
	// read stdin
	/*
	size_t sz = sizeof(buff);
	int ch, extra;
	if (!feof(stdin)) {
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
	} else */
	if(argc == 2) {
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
	for(i=0; i<strlen(buff)+1; i++) {
		if (buff[i] == '\n' || buff[i] == '\r') {
			buff[i] = '\0';
			break;
		}
	}
	
	// TODO: remove double and single quotes at the end and beginning of input.
	//       some shells might pass them on to the programm.

	#if DEBUG > 0
	fprintf(logfile, "===========================\n");
	fprintf(logfile, "stdin: %s\n", buff);
	fprintf(logfile, "stdin: %d, buff: %d\n", (int) strlen(argv[1]), (int) strlen(buff));
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
		if (reminder[counter] == '/' && found_slash == -1) 
			found_slash = counter;
		if (reminder[counter] == '?' && found_questionmark == -1) 
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
		path[strlen(reminder)+1-found_slash] = '\0';
	}
	
	// authority and query
	if (found_slash == -1 && found_questionmark > 0) {
		strncpy(authority, reminder, strlen(reminder));
		authority[found_questionmark] = '\0';
		strncpy(query, reminder+found_questionmark+1, strlen(reminder)+1-found_questionmark);
		query[strlen(reminder)+1-found_questionmark] = '\0';
	}
	
	// authority and path and query
	if (found_slash > 0 && found_questionmark > 0) {
		strncpy(authority, reminder, strlen(reminder));
		authority[found_slash] = '\0';
		strncpy(path, reminder+found_slash, found_questionmark-found_slash);
		path[found_questionmark-found_slash] = '\0';
		strncpy(query, reminder+found_questionmark+1, strlen(reminder)+1-found_questionmark);
		query[strlen(reminder)+1-found_questionmark] = '\0';
	}
	
	// urldecode params
	char *query_escaped = malloc(strlen(query)+1);
	urldecode2(query_escaped, query);
	
	// display parsed uri
	#if DEBUG > 0
	fprintf(logfile, "scheme    (%d): '%s'\n", found, scheme);
	fprintf(logfile, "authority (%d): '%s'\n", (int) strlen(authority), authority);
	fprintf(logfile, "path      (%d): '%s'\n", found_slash, path);
	fprintf(logfile, "query     (%d): '%s'\n", found_questionmark, query_escaped);
	#endif

	// check for allowed chars in authority and path
	int res_authority;
	res_authority = findchar(allowed, (const char*) authority);
	if (res_authority != 0) {
		printerr("Unallowed character in authority\n");
		return UNALLOWED_CHAR_IN_AUTHORITY;
	}
	
	// check if there are disallwoed chars in path
	int res_path;
	char* allowedp = malloc(sizeof(char) * strlen(allowed)+2);
	strcpy(allowedp, allowed);
	strcat(allowedp, "/");
	res_path = findchar(allowedp, (const char*) path);
	if (res_path != 0) {
		printerr("Unallowed character in path\n");
		return UNALLOWED_CHAR_IN_PATH;
	}

	// find the appropriate config in the ini file
	// try to read ini file
	char section[STDIN_MAX] = "";
	strcpy(section, authority);
	//strcat(section, "/");
	strcat(section, path);
	config.section = section;
	int retp = ini_parse(ini_file, dumper, &config);
	
	// check if we failed to open the file
	if (retp == -1) {
		fprintf(stderr, "Failed to open the ini file, trying to create it:\n");
		fprintf(stderr, "%s\n", ini_file);
		return INI_PERMISSION_DENIED;
	}
	
	if (config.found == 0) {
		perror("unknown ini section or key");
		return UNKNOWN_INI_KEY;
	}
	
	#if DEBUG > 0
	fprintf(logfile, "section:        %s\n", config.section);
	fprintf(logfile, "exe:            %s\n", config.exe);
	fprintf(logfile, "default_path:   %s\n", (strcmp(config.default_path, "") == 0) ? "(unset)" :  config.default_path);
	fprintf(logfile, "allowed_params: %s\n", (strcmp(config.allowed_params, "") == 0) ? "(unset)" :  config.allowed_params);
	fprintf(logfile, "path_params:    %s\n", (strcmp(config.path_params, "") == 0) ? "(unset)" :  config.path_params);
	fprintf(logfile, "params_prepend: %s\n", (strcmp(config.params_prepend, "") == 0) ? "(unset)" :  config.params_prepend);
	fprintf(logfile, "params_append:  %s\n", (strcmp(config.params_append, "") == 0) ? "(unset)" :  config.params_append);
	#endif
	
	// split parameters by ',' into a char** array so we can check 
	// against configuration
	struct str_array a_allowed_params = str_array_split(strdup(config.allowed_params), ",");
	
	#if DEBUG > 0
	str_array_debug("allowed_params", a_allowed_params);
	#endif
	
	// split the path_params accordingly into an array by ','
	struct str_array a_path_params = str_array_split(strdup(config.path_params), ",");

	#if DEBUG > 0
	str_array_debug("path_params", a_path_params);
	#endif
	
	// finally split the passed parameters
	struct str_array a_query_escaped = str_array_split(strdup(query_escaped), "&");
	
	#if DEBUG > 0
	str_array_debug("query_escaped", a_query_escaped);
	#endif
	
	int cc;
	char* default_path = malloc(sizeof(char*) * (strlen(config.default_path)+1));
	strcpy(default_path, config.default_path);
	// replace all '\\' in paths with '/'.
	if (strcmp(default_path, "") != 0) {
		for(cc=0; cc<strlen(default_path); cc++)
			if (default_path[cc] == '\\')
				default_path[cc] = '/';
		//printf("New default path: %s\n", default_path);
	}
	
	// check that all parameters in a_query_escaped are also contained in
	// a_allowed_params to make sure no unwanted parameters are used
	int unvalidated_params = 0;
	const int unvalidated_counter_start = MAX_UNVALIDATED_PARAMETERS;
	int unvalidated_counter = unvalidated_counter_start;
	char **unvalidated = malloc(sizeof(char *) * MAX_UNVALIDATED_PARAMETERS);
	int unvalidated_length = 0;
	char *unvalidated_buff = malloc(sizeof(char *) * STDIN_MAX);
	char clean_path[PATH_MAX];
	unvalidated_buff[0] = '\0';
	for (i=0; i<a_query_escaped.length; i++) {
		int is_param = 0;
		if (a_query_escaped.items[i][0] == '-' ||
		    a_query_escaped.items[i][0] == '/')
			is_param = 1;
			
		int res = find_param(a_query_escaped.items[i], &a_allowed_params);
		#if DEBUG > 1
		fprintf(logfile, "    %d: [%d]: '%s'\n", i, res, a_query_escaped.items[i]);
		#endif
		
		// failed to find parameter in allowd parameters
		if (res == 0 && is_param) {
			unvalidated_params = 1;
			#if DEBUG > 0
			int stack = unvalidated_counter-unvalidated_counter_start*-1;
			fprintf(logfile, "Stack %d\n", stack);
			#endif
			unvalidated[unvalidated_length++] = a_query_escaped.items[i];
			unvalidated_counter--;

			if (unvalidated_length != 1)
				strcat(unvalidated_buff, ", ");
			
			strcat(unvalidated_buff, unvalidated[unvalidated_length-1]);
		}
		
		if (unvalidated_counter < 1) {
			char err[MAX_UNVALIDATED_PARAMETERS*30] = "";
			sprintf(err, "Number of unvalidated parmaeters exhausetd (num: %d)\n",
			        unvalidated_counter_start);
			printerr(err);
			return TOO_MANY_PARAMETERS;
		}
		
		// check if it is a path parameter
		res = find_param(a_query_escaped.items[i], &a_path_params);
		
		// This is a path parameter, clean it up
		if (res == 1) {
			
			// extract the value from the parameter
			char* value = malloc(sizeof(char *) * strlen(a_query_escaped.items[i]));
			int has_value = get_value_from_argument(a_query_escaped.items[i], value);
			if (has_value == 0) {
				if (a_query_escaped.length > i+1) {
					value = a_query_escaped.items[i+1];
				}
			}
			#if DEBUG > 0
			fprintf(logfile, "path param value: '%s'\n", value);
			#endif
			
			if (value[0] == '.' && value[1] == '.') {
				#if DEBUG > 0
				fprintf(logfile, "path starts with '..': '%s'\n", value);
				#endif
				return INVALID_PATH;
			}
			
			// replace all '\\' in paths with '/'.
			for(cc=0; cc<strlen(value); cc++)
				if (value[cc] == '\\')
					value[cc] = '/';
			
			// check for '.' and '..' in paths and normalize (remove) them.
			errno = 0; // FIXME: we have unhandled errors down here, so i need to reset errno. Typically EINVAL=22
			realpath(value, clean_path);
			if (errno != 0) {
				perror("realpath failed");
				#if DEBUG > 0
				fprintf(logfile, "realpath() failed to clean up path [%d]: '%s'\n", errno, value);
				//hex_dump("path value: ", value, strlen(value));
				#endif
				return INVALID_PATH;
			}
			
			// default path configured, we must make sure the path parameter 
			// starts with this value
			if (strcmp(config.default_path, "") != 0) {
				int start = starts_with(default_path, value);
				#if DEBUG > 0
				fprintf(logfile, "Path starts with default_path: %d\n", start);
				#endif
				
				if (start != 0) {
					char err[STDIN_MAX] = "";
					sprintf(err, "Path in parameter not inside the configured "
					             "'default_path': %s\n",
							default_path);
					printerr(err);
					return PATH_NOT_ALLOWED;
				}
			}
			
			// set cleaned path back to query string
			if (has_value) {
				char* key = find_key(a_query_escaped.items[i]);
				printf("key: %s\n", key);
				char* tmpbuff = malloc(sizeof(char) * strlen(a_query_escaped.items[i])+1 );
				strcpy(tmpbuff, key);
				strcat(tmpbuff, "=");
				strcat(tmpbuff, clean_path);
			} else {
				//printf("%s\n", clean_path);
				a_query_escaped.items[i+1] = clean_path;
			}
		}
	}
	
	// if there are parameters passed in with the query string which are not 
	// in allowed_params abort
	if (unvalidated_params == 1) {
		char *err_buff = malloc(sizeof(char *) * STDIN_MAX);
		sprintf(err_buff, "Unvalidated parameter submitted: %s\n", unvalidated_buff);
		printerr(err_buff);
		return UNALLOWED_PARAM;
	}
	
	// create command line arguments from a_query_escaped
	int numargs = 0;
	const char* myargs[100];
	
	// this is a hack, so myrgs in spawnve is never empty
	myargs[numargs++] = " "; 
	myargs[numargs] = NULL;

	char* cmd = malloc(sizeof(char) * STDIN_MAX*2);
	strcpy(cmd, config.exe);
	if (strcmp(config.params_prepend, "") != 0) {
		strcat(cmd, " ");
		strcat(cmd, config.params_prepend);
		
		struct str_array p = str_array_split(strdup(config.params_prepend), " ");
		for (i=0; i<p.length; i++)
			myargs[numargs++] = p.items[i];
		str_array_destroy(p);
	}
	
	for (i=0; i<a_query_escaped.length; i++) {
		strcat(cmd, " ");
		strcat(cmd, a_query_escaped.items[i]);
		myargs[numargs++] = a_query_escaped.items[i];
	}
	
	if (strcmp(config.params_append, "") != 0) {
		strcat(cmd, " ");
		strcat(cmd, config.params_append);
		
		struct str_array p = str_array_split(strdup(config.params_append), " ");
		for (i=0; i<p.length; i++)
			myargs[numargs++] = p.items[i];
		str_array_destroy(p);
	}
	
	// check if the file is executable
	struct stat sb;
	if (stat(config.exe, &sb) == 0 && sb.st_mode & S_IXUSR) {
		#if DEBUG > 0
		fprintf(logfile, "Program '%s' is executable\n", config.exe);
		#endif
		;
	} else {
		#if DEBUG > 0
		fprintf(logfile, "Program '%s' is not executable\n", config.exe);
		#endif
		fprintf(stderr, "Program '%s' is not executable\n", config.exe);
		return PROGRAM_IS_NOT_EXECUTABLE;
	}
	
	// TODO: add a transformation mechanism for parameter names so that we can 
	//       use different parameter names on the command line than in the URI
	//       example:         proto://exe/profile?username=foo
	//       transform username to /u
	//       transfomed cmd:  profile.exe /u foo
	

	cmd[0] = '\0';
	strcat(cmd, config.exe);
	for (i=0; i<a_query_escaped.length; i++) {
		strcat(cmd, " ");
		strcat(cmd, a_query_escaped.items[i]);
	}
	
	fprintf(logfile, "Executing: %s\n", cmd);

	// run the command, spawn a new process and end this application
	int proc = 0;
	//printf("%s\n", config.exe);
	//printf("%s\n", myargs[0]);
	proc = spawnve(P_NOWAIT, config.exe, myargs, (const char * const *) environ);
	//proc = spawnle(P_NOWAIT, config.exe, "", NULL, (const char * const *) environ);
	
	if (proc == -1) {
		#if DEBUG > 0
		fprintf(logfile, "spanv: %d, error: %d\n", proc, errno);
		#endif
		fprintf(stderr, "exe: %s\n", config.exe);
		perror("spawnv() error");
		fprintf(stderr, "Make sure the program is in your search path. Otherwise define fully qualified path in the ini file.\n");
	}
	//system(cmd);

	#if DEBUG > 0 && LOG_TO_FILE == 1
	fclose(logfile);
	#endif

	return OK;
}
