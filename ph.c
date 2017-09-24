#include "ph.h"

//const char* empty = "";
extern char **environ;

char* errstr[255] = { NULL };

// can hold one ini file entry
#define DEFAULT_CONFIG { "", "", "", "", "", "", "", "", 0}
typedef struct {
	const char* section; // the section we are searchin for
	const char* default_path;
	const char* allowed_params;
	const char* path_params;
	const char* params_prepend;
	const char* params_append;
	const char* replace;
	const char* exe;
	int found; // 1 if the section was found. initialize it to 0 otherwise
} configuration;

/**
 * Display usage
 */
void usage(void) {
	printf(usage_str);
}

/**
 * Logging to ph.log
 */
int loglevel = 5; // 0 will disable logging
char logbuffer[4096];
void writelog(int level, char* str) {
	
	char prefix[20] = "";
	int i = 1;
	for(i=2; i<20; i++) {
		if (i > level) break;
		strcat(prefix, "\t");
	}
	
	if (loglevel < level)
		return;
	fprintf(logfile, "%s%s\r\n", prefix, str);
}

int display_error(int code) {
	char params[25] = "";
	sprintf(params, "error.html?%d", code);
	char *myargs[5] = {
		"/c"
		"hh.exe",
		"-800",
		params,
		NULL
	};
	char exe[4096] = "";
	strcat(exe, getenv("windir"));
	strcat(exe, "\\System32\\cmd.exe");
	spawnve(P_NOWAIT, exe, myargs, environ);
	//printf("spawnv %d\n", ret);
	
	// TODO: return message to stderr
	
	return code;
}

extern char **environ;
int main(int argc, char** argv, char **envp) {
	environ = envp;
	
	errstr[0] = "Ok";
	errstr[1] = "Argument 1 missing.";
	errstr[2] = "Failed to find current directory. getcwd() or realpath() failed to resolve the app directory.";
	errstr[3] = "Failed to create ph.ini, permission denied.";
	errstr[4] = "Failed to create ph.ini, unable to write to file.";
	errstr[5] = "Failed to parse ini file.";
	errstr[6] = "Wrong path or program is not executable.";

	errstr[128] = "Failed to parse URI, protocol missing.";
	errstr[129] = "Failed to parse URI, authority missing.";
	errstr[130] = "Failed to parse URI, error in query string.";
	errstr[131] = "Failed to parse URI, error in fragment.";
	//errstr[] = "";
	
	int i = 0;
	int l = 0;
	
	// fin the current directory of the executable
	char *dir = malloc(sizeof(char*) * (MAX_CWD_LENGTH+1));
	int r = exedir(argv[0], dir);
	if (r != 0) {
		fprintf(stderr, "Failed to find current directory, error: %d\n", r);
		return display_error(NO_CURRENTDIR);
	}
	
	// open log file
	char log_file[MAX_CWD_LENGTH];
	strcpy(log_file, dir);
	strcat(log_file, "ph.log");
	logfile = fopen(log_file, "ab+");
	
	// check if we have an ini file, if not, create it
	char ini_file[MAX_CWD_LENGTH];
	strcpy(ini_file, dir);
	strcat(ini_file, INI_FILE_NAME);
	if( access(ini_file, F_OK) == -1 ) {
		// file doesn't exist, create it
		int ret = create_ini(ini_file);
		return display_error(ret);
	}
	
	// check input
	if(argc != 2) {
		perror("argument 1 with uri missing");
		usage();
		return display_error(NO_INPUT);
	}

	// logging uri
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	sprintf(logbuffer, "\r\n===> %d-%02d-%02d %02d:%02d:%02d, URI: '%s'", 
	        tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, 
			tm.tm_min, tm.tm_sec,  argv[1]);
	writelog(1, logbuffer);

	sprintf(logbuffer, "Current working dir: %s", dir);
	writelog(2, logbuffer);
	sprintf(logbuffer, "Current ini file:    %s", ini_file);
	writelog(2, logbuffer);
	
	// parse uri
	int ret = 0;
	int res;
	//struct t_uri uri_parsed = {uri, empty, empty, empty, empty, empty, {-1, -1, -1, -1, -1, -1, -1}};
	struct t_uri uri_parsed = uriparse_create(argv[1]);
	res = uriparse_parse(argv[1], &uri_parsed);
	if (res != 0) {
		ret = 127+res;
		sprintf(logbuffer, "URI Parser Error %d, %s", res, argv[1]);
		writelog(1, logbuffer);
		return display_error(ret);
	}
	
	if (loglevel > 2) {
		sprintf(logbuffer, "Start      [%d]", uri_parsed.pos[FOUND_START]); writelog(3, logbuffer);
		sprintf(logbuffer, "Proto      [%d]", uri_parsed.pos[FOUND_PROTO]); writelog(3, logbuffer);
		sprintf(logbuffer, "Authority  [%d]", uri_parsed.pos[FOUND_AUTHORITY]); writelog(3, logbuffer);
		sprintf(logbuffer, "Path       [%d]", uri_parsed.pos[FOUND_PATH]); writelog(3, logbuffer);
		sprintf(logbuffer, "Query      [%d]", uri_parsed.pos[FOUND_QUERY]); writelog(3, logbuffer);
		sprintf(logbuffer, "Fragment   [%d]", uri_parsed.pos[FOUND_FRAGMENT]); writelog(3, logbuffer);
		sprintf(logbuffer, "End        [%d]", uri_parsed.pos[FOUND_END]); writelog(3, logbuffer);

		sprintf(logbuffer, "proto:     '%s'", uri_parsed.proto); writelog(3, logbuffer);
		sprintf(logbuffer, "authority: '%s'", uri_parsed.authority); writelog(3, logbuffer);
		sprintf(logbuffer, "path:      '%s'", uri_parsed.path); writelog(3, logbuffer);
		sprintf(logbuffer, "query:     '%s'", uri_parsed.query); writelog(3, logbuffer);
		sprintf(logbuffer, "fragment:  '%s'", uri_parsed.fragment); writelog(3, logbuffer);
	}
	
	// parse ini file
	l = strlen(uri_parsed.authority) + strlen(uri_parsed.path) + 2;
	char *section = (char *) malloc(sizeof(char*) * l);
	section[0] = 0;
	strcat(section, uri_parsed.authority);
	strcat(section, "/");
	strcat(section, uri_parsed.path);

	//printf("section: %s\n", section);
	sprintf(logbuffer, "Reading ini section: %s", section);
	writelog(1, logbuffer);
	
	// read the config 
	configuration config = DEFAULT_CONFIG;
	config.section = section;
	int retp = ini_parse(ini_file, ini_callback, &config);
	
	sprintf(logbuffer, "ini_parse() return code: %d", retp);
	writelog(2, logbuffer);
	
	if (retp != 0) {
		return display_error(INI_PARSE_ERR);
	}

	if (loglevel > 2) {
		sprintf(logbuffer, "section: %s", config.section); writelog(3, logbuffer);
		sprintf(logbuffer, "default_path: %s", config.default_path); writelog(3, logbuffer);
		sprintf(logbuffer, "allowed_params: %s", config.allowed_params); writelog(3, logbuffer);
		sprintf(logbuffer, "path_params: %s", config.path_params); writelog(3, logbuffer);
		sprintf(logbuffer, "params_prepend: %s", config.params_prepend); writelog(3, logbuffer);
		sprintf(logbuffer, "params_append: %s", config.params_append); writelog(3, logbuffer);
		sprintf(logbuffer, "replace: %s", config.replace); writelog(3, logbuffer);
		sprintf(logbuffer, "exe: %s", config.exe); writelog(3, logbuffer);
		sprintf(logbuffer, "found: %d", config.found); writelog(3, logbuffer);

		if (loglevel > 2) {
			for(i=0; i<uri_parsed.nvquery.length; i++) {
				sprintf(logbuffer, "[%d] '%s'='%s'", i, uri_parsed.nvquery.items[i].key, uri_parsed.nvquery.items[i].value);
				writelog(4, logbuffer);
			}
		}
	}
	
	// check if the configuration defines an exe that is executable
	//printf("exe: %s\n", config.exe);
	struct stat sb;
	if (stat(config.exe, &sb) == 0 && sb.st_mode & S_IXUSR) {
		sprintf(logbuffer, "Program '%s' is executable", config.exe);
		writelog(2, logbuffer);
	} else {
		sprintf(logbuffer, "Program '%s' is not executable", config.exe);
		writelog(1, logbuffer);
		fprintf(stderr, "%s\n", logbuffer);
		return display_error(PROGRAM_IS_NOT_EXECUTABLE);
	}
	//return 0;
	
	// TODO: do file content replacement
	// TODO: check env parameters
	// TODO: replace url parameter names with cmd args
	// TODO: check if document is within default path
	// TODO: check if values of path parameters are inside default path
	// TODO: add prepend/append parameters
	// TODO: run command

	#define NUMARGS (3)
	char *myargs[NUMARGS] = {
		"/c",
		(char *) config.exe,
		NULL
	};
	char exe[4096] = "";
	strcat(exe, getenv("windir"));
	strcat(exe, "\\System32\\cmd.exe");

	sprintf(logbuffer, "running command: %s", exe);
	for(i=0; i<NUMARGS-1; i++) {
		strcat(logbuffer, " ");
		strcat(logbuffer, myargs[i]);
	}
	writelog(1, logbuffer);

	// we run the application in a new process, without waiting for it to exit.
	// this means we can't capture the exit code. Which might be helpful with 
	// startup errors but in other situations this is a useless information
	// because the application can ugracefully exit later on which has nothing 
	// to do with us starting it.
	ret = spawnve(P_NOWAIT, exe, myargs, environ);
	if (ret < 0) {
		sprintf(logbuffer, "spawnve() returned error: %d", ret);
		writelog(1, logbuffer);
		fprintf(stderr, "%s\n", logbuffer);
	}
	
	return OK;
}

/**
 * callback function for ini file parser
 */
static int ini_callback(void* user, const char* section, const char* name,
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
		} else if (MATCH(section, "replace")) {
			pconfig->replace = strdup(value);
			pconfig->found = 1;
		} else {
			return 0;
		}
	}
	// printf("search section: %s\n", (const char *) pconfig->section);

	return 1;
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
			fprintf(stderr, "Permission denied!\n");
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