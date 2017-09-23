#include "ph.h"

//const char* empty = "";
extern char **environ;

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
	if (loglevel < level)
		return;
	fprintf(logfile, "%s\r\n", str);
}

int main(int argc, char** argv, char **envp) {
	
	//int i = 0;
	int l = 0;
	
	// fin the current directory of the executable
	char *dir = malloc(sizeof(char*) * (MAX_CWD_LENGTH+1));
	int r = exedir(argv[0], dir);
	if (r != 0) {
		fprintf(stderr, "Failed to find current directory, error: %d\n", r);
		return 1;
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
		return ret;
	}
	
	
	sprintf(logbuffer, "Current working dir: %s", dir);
	writelog(1, logbuffer);
	sprintf(logbuffer, "Current ini file:    %s", ini_file);
	writelog(1, logbuffer);

	// check input
	if(argc != 2) {
		perror("argument 1 with uri missing");
		usage();
		return NO_INPUT;
	}

	// logging uri
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	sprintf(logbuffer, "===> %d-%02d-%02d %02d:%02d:%02d, URI: '%s'", 
	        tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, 
			tm.tm_min, tm.tm_sec,  argv[1]);
	writelog(1, logbuffer);
	
	// parse uri
	int ret = 0;
	int res;
	//struct t_uri uri_parsed = {uri, empty, empty, empty, empty, empty, {-1, -1, -1, -1, -1, -1, -1}};
	struct t_uri uri_parsed = uriparse_create(argv[1]);
	res = uriparse_parse(argv[1], &uri_parsed);
	if (res != 0) {
		ret = 127+res;
		sprintf(logbuffer, "Parser Error %d, %s\n", res, argv[1]);
		writelog(1, logbuffer);
		return ret;
	}
	
	// parse ini file
	l = strlen(uri_parsed.proto) + strlen(uri_parsed.authority) + 2;
	char *section = (char *) malloc(sizeof(char*) * l);
	section[0] = 0;
	strcat(section, uri_parsed.proto);
	strcat(section, "/");
	strcat(section, uri_parsed.authority);

	sprintf(logbuffer, "Reading ini section: %s\n", section);
	writelog(1, logbuffer);
	
	// read the config 
	configuration config = DEFAULT_CONFIG;
	config.section = section;
	int retp = ini_parse(ini_file, ini_callback, &config);
	
	sprintf(logbuffer, "ini_parse(): %d\n", retp);
	writelog(2, logbuffer);
	
	// TODO: do file content replacement
	// TODO: check env parameters
	// TODO: replace url parameter names with cmd args
	// TODO: check if document is within default path
	// TODO: check if values of path parameters are inside default path
	// TODO: add prepend/append parameters
	// TODO: run command
	
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