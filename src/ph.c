#include "ph.h"

//const char* empty = "";
extern char **environ;

/**
 * Display usage
 */
void usage(void) {
	printf(usage_str);
}

/**
 * Logging to ph.log
 */
int loglevel = 0; // 0 will disable logging
char logbuffer[4096];
char log_file[MAX_CWD_LENGTH];

char encodek[20];

char * my_itoa(int i) {
  char * res = (char*) malloc(8*sizeof(int));
  sprintf(res, "%d", i);
  return res;
}
 
void writelog(int level, char* str) {
	if (level == 0)
		return;
	
	char prefix[20] = "";
	int i = 1;
	for(i=2; i<20; i++) {
		if (i > level) break;
		strcat(prefix, "\t");
	}
	
	if (loglevel < level)
		return;

	if (logfile != NULL)
		fprintf(logfile, "%s%s\r\n", prefix, str);
	else
		fprintf(stderr, "%s%s\r\n", prefix, str);
}

int file_exists(char* file) {
if( access(file, F_OK) != -1 )
		return 0;
	return 1;
}

int display_error(int code) {
	char params[MAX_CWD_LENGTH];
	
	sprintf(params, "%s/error.html?%d", getenv("PH_HOME"), code);
	const char* myargs[5] = {
		"/C"
		"hh.exe",
		"-800",
		params,
		NULL
	};
	
#ifdef __MINGW32__
	char exe[4096] = "";
	strcat(exe, getenv("windir"));
	strcat(exe, "\\System32\\cmd.exe");
	spawnve(P_DETACH, exe, (char* const*) myargs, (char* const*) environ);
	
#else
	// FIXME: add global handling
	printf("We would isplay an error here\n");
#endif
	
	//define_error_messages();
	//fprintf(stderr, "%s\n", errstr[code]);
	sprintf(logbuffer, "Error: %d, %s", code, errstr[code]);
	fprintf(stderr, logbuffer);
	if (logfile != NULL)
		writelog(1, logbuffer);
	
	return code;
}

int replace(const char* file, const char* regex) {
	
	// open file
	FILE *f = fopen(file, "rb");
	if (f == NULL) {
		fprintf(stderr, "Failed to open file %s for replacements.\n", file);
		return FAILED_TO_OPEN_REPLACE_FILE;
	}
	
	fseek(f, 0, SEEK_END);
	long fsize = ftell(f);
	fseek(f, 0, SEEK_SET);  //same as rewind(f);

	char *string = (char*) malloc(fsize + 1);
	
	if (string == NULL)
		return FAILED_TO_ALLOC_MEM_FOR_FILE;
	
	fread(string, fsize, 1, f);
	fclose(f);

	string[fsize] = 0;
	
	const char* result;
	//const char* reg = "pre/[0-9]+/NNNN/m";
	
	//printf("regex: %s\n", regex);
	result = regreplace(regex, string);
	
	if (result == NULL) {
		printf("Failed to execute regex: %d\n", regerrno);
		return FAILED_TO_PARSE_REGEX;
	}
	
	//printf("%s\n", result);
	
	// write result
	f = fopen(file, "wb");
	fwrite(result, 1, strlen(result), f);
	fclose(f);
	
	return OK;
}

/*
int expenv(char** str) {
	int i, o, open, start;
	int l = strlen(str[0]);
	open = 0; i = 0; o = 0; start = 0;
	
	char* out = (char*) malloc(sizeof(char*) * 1);
	if (out == NULL)
		return 2;
	out[0] = 0;
	
	for (i=0; i<l; i++) {
		if (i > 0 && str[0][i] == '{' && str[0][i-1] == '$') {
			//printf("Begin: %d\n", i);
			open = 1;
			o--;
			start = i;
			continue;
		}
		
		if (str[0][i] == '}') {
			//printf("End:   %d\n", i);
			open = 0;
			
			int len = (i-start-1);
			
			char* varname = (char *) malloc(sizeof(char*) * (len+1));
			if (varname == NULL)
				return 2;
			strncpy(varname, *(str)+start+1, len);
			varname[len] = 0;
			char* value = getenv(varname);
			
			if (value == NULL)
				return 1;
			
			//printf("Var(%d): %s=%s\n", len, varname, value);
			
			out = (char *) realloc(out, o+strlen(value)+1);
			out[o] = 0;
			strcat(out, value);
			o+= strlen(value);
			
			continue;
		}
		
		if (open == 0) {
			out = (char *) realloc(out, o+1);
			if (out == NULL)
				return 2;
			out[o++] = str[0][i];
			out[o] = 0;
		}
	}
	
	//printf("out: %s\n", out);
	*str = out;
	
	return 0;
}
*/

int encode_file(int argc, char** argv) {
	
	char* infile = argv[2];
	char* outfile;
	if (argc > 3)
		outfile = argv[3];
	else {
		int l = strlen(infile);
		outfile = (char*) malloc(sizeof(char*) * (l + 1));
		strcpy(outfile, infile);
		outfile[l-4] = '.';
		outfile[l-3] = 'd';
		outfile[l-2] = 'a';
		outfile[l-1] = 't';
	}
	
	FILE *f = fopen(infile, "rb");
	if (f == NULL)
		return 1;
	
	fseek(f, 0, SEEK_END);
	long fsize = ftell(f);
	fseek(f, 0, SEEK_SET);  //same as rewind(f);
	//printf("%ld\n", fsize);
	char *string = (char*) malloc(fsize);
	fread(string, fsize, 1, f);
	//string[fsize] = 0;
	fclose(f);
	
	char *result;
	//printf("fsize before: %ld\n", fsize);
	transcode_str(string, &fsize, &result, encodek);
	//printf("fsize after: %ld\n", fsize);
	
	f = fopen(outfile, "wb");
	fwrite(result, 1, fsize, f);
	if (f == NULL)
		return 2;
	
	fclose(f);
	
	return OK;
}
	
int test_regex(int argc, char** argv) {
	
	// open file
	FILE *f = fopen(argv[3], "rb");
	if (f == NULL) {
		fprintf(stderr, "Failed to open file.\n");
		return 2;
	}
	
	fseek(f, 0, SEEK_END);
	long fsize = ftell(f);
	fseek(f, 0, SEEK_SET);  //same as rewind(f);

	char *string = (char*) malloc(fsize + 1);
	fread(string, fsize, 1, f);
	fclose(f);

	string[fsize] = 0;
	
	const char* result;
	//const char* reg = "pre/[0-9]+/NNNN/m";
	
	printf("regex: %s\n", argv[2]);
	result = regreplace(argv[2], string);
	
	if (result == NULL) {
		printf("Failed to execute regex: %d\n", regerrno);
		return 3;
	}
	
	printf("%s\n", result);
	
	return OK;
}


#ifndef PH_NO_MAIN
//char **environ;
int main(int argc, char** argv, char **envp) {
	
	// setup globals
	environ = envp;
	logfile = NULL;
	
	int i = 0;
	int ii = 0;
	int l = 0;
	int ret = 0;
	int res;
	int retp = 0;
	long fsize;
	
	// key for encoding the config file
	encodek[0] = '3';
	encodek[1] = 'f';
	encodek[2] = 'J';
	encodek[3] = '%';
	encodek[4] = 'd';
	encodek[5] = '6';
	encodek[6] = '5';
	encodek[7] = ',';
	encodek[8] = '$';
	encodek[9] = '/';
	encodek[10] = '@';
	encodek[11] = 'd';
	encodek[12] = 'f';
	encodek[13] = 'h';
	encodek[14] = 's';
	encodek[15] = '(';
	encodek[16] = '3';
	encodek[17] = 'l';
	encodek[19] = 0;
	
	/**
	 * define error strings
	 *
	 * an aray with all error strings corepsonding to error codes 
	 * is defined in lib/errstr.c. These strings are used to throw 
	 * a bit more meaningful error messages in case we need to abort
	 * the program. The program will always also use the error code as exit 
	 * code bit will send theses error strings to stderr.
	 */
	define_error_messages();
	
	/**
	 * find the current directory of the executable.
	 *
	 * This is a cross platform way to find the installation directory of 
	 * the executable. It is tested on Mingw64, Linux and OSX (gcc/clang).
	 *
	 * The installation directory is used to find a configuration file if
	 * there is none provided via argv[2]. 
	 */
	char *dir = (char*) malloc(sizeof(char*) * (MAX_CWD_LENGTH+1));
	int r = exedir(argv[0], dir);
	if (r != 0) {
		fprintf(stderr, "Failed to find current directory, error: %d\n", r);
		return display_error(NO_CURRENTDIR);
	}
	
	/**
	 * set an environment Variable with our directroy path
	 *
	 * This environment variable can be used in the config file in the form of 
	 * ${env.PH_HOME}. It is also used to find the error.html file when 
	 * displaying error mesages to the user. error.html should be installed in 
	 * the same folder as tis executable.
	 */
	char env[MAX_CWD_LENGTH+1+9] = {0};
	strcat(env, "PH_HOME=");
	strcat(env, dir);
	env[strlen(env)-1] = 0;
	putenv(env);
	
	
	// check command line parameters for mode
	
	/**
	 * encode config file
	 *
	 * PROGNAME_SHORT_EXT -e <config_file> [out_file]
	 *
	 * encode the config file. if parameter 2 is missing, the encoded file 
	 * will be stored in the same folder as the original file with a file 
	 * ending `.dat`.
	 */
	if (argc > 1 && strcmp(argv[1], "-e") == 0)
		return encode_file(argc, argv);
	
	/**
	 * test regex
	 *
	 * PROGNAME_SHORT_EXT -r </regex/replace/> <file>
	 *
	 * it can be useful to test a regular expression before putting it into the 
	 * config file. This command line switch will allow to do this.
	 *
	 * be careful, in your shell, you might need to use escape characters not 
	 * needed in the config file. in a bas, you must escape '$' with '\$', 
	 * however you do not have to escape it in the config file.
	 */
	if (argc > 1 && strcmp(argv[1], "-r") == 0)
		return test_regex(argc, argv);

	/**
	 * figure out where to config file is
	 *
	 * we take several options into account
	 * 1. if argv[2] is present, read the config file from this location
	 *    a. check if the file name starts with `http|ftp', if so use the cURL 
	 *       client to download it into a temp folder
	 *    b. if it is a local file remeber it's location
	 * 2. else read it from the program folder.
	 *    a. check if there is a `.dat` file present. if so use it
	 *       the `.dat` file is an encoded config file
	 *    b. else check if there is a `.ini` file present,  if so read it
	 *
	 * Whatever prooves to be true, the filename of the config file is stored 
	 * in the `ini_file` variable for later read.
	 */
	char ini_file[MAX_CWD_LENGTH];
	int remote_config = 0;
	// FIXME: search for ph.dat if no ph.ini is found
	if (argc == 3) {
		strcpy(ini_file, argv[2]);
		
		// check if this is a remote file
		if ( (ini_file[0] == 'h' && ini_file[1] == 't' && ini_file[2] == 't' && ini_file[3] == 'p') ||
			 (ini_file[0] == 'f' && ini_file[1] == 't' && ini_file[2] == 'p')) {
			
			// download http/https/ftp/ftps
			remote_config = 1;
			res = getfile(argv[2]);
			
			if (res != 0) {
				return display_error(FAILED_TO_DOWNLOAD_CONFIG);
			}
			
			// get the location of the downloaded file
			char* tempfile;
			tempfilepath(&tempfile);
			strcpy(ini_file, tempfile);
		}
	} else {
		strcpy(ini_file, dir);
		strcat(ini_file, str(PROGNAME_SHORT));
		strcat(ini_file, ".dat");
		
		if (file_exists(ini_file) != 0) {
			strcpy(ini_file, dir);
			strcat(ini_file, str(PROGNAME_SHORT));
			strcat(ini_file, ".ini");
			
			// check if we have an ini file, if not, create it
			if(file_exists(ini_file) != 0) {
				ret = create_ini(ini_file);
				printf("Example configuration file created in: %s\n", ini_file);
				return OK;
			}
		}
	}
	//printf("ini_file: %s\n", ini_file);
	
	/**
	 * Read the config file
	 * 
	 * - read it from disk. Check if it is encoded. If so unencode it.
	 * - Then read the global application configuration found in [_global] 
	 *   in the ini file.
	 * - Expand env varibles in the global configuration.
	 * - validate values (sizes must be sane integer values, etc.)
	 */
	FILE *f = fopen(ini_file, "rb");
	if (f == NULL)
		return display_error(FAILED_TO_OPEN_CONFIG);
	
	fseek(f, 0, SEEK_END);
	fsize = ftell(f);
	fseek(f, 0, SEEK_SET);  //same as rewind(f);
	char *string = (char*) malloc(fsize);
	fread(string, fsize, 1, f);
	fclose(f);
	
	// read and unencode file if encoded
	char *ini_content;
	if (is_encoded(string) == 1) {
		//printf("encoded\n");
		//printf("fsize 1: %ld\n", fsize);
		res = transcode_str(string, &fsize, &ini_content, encodek);
		//printf("fsize 2: %ld\n", fsize);
	} else {
		ini_content = string;
	}
	
	// read the global configuration
	//printf(ini_content);
	global_configuration cfg = DEFAULT_GCONFIG;
	retp = ini_parse_string(ini_content, global_callback, &cfg);
	
	if (retp != 0) {
		sprintf(logbuffer, "error parsing ini file at line %d.", retp); 
		writelog(1, logbuffer);
		return display_error(INI_PARSE_ERR);
	}
	
	// expand variables in the configuration
	struct nvlist_list tmp_nvlist = nvlist_create(0);
	char* tmp_log_path = (char*) malloc(sizeof(char*) * (strlen(cfg.log_path)+1));
	strcpy(tmp_log_path, cfg.log_path);
	ret = expand_vars(&tmp_log_path, &tmp_nvlist);
	cfg.log_path = tmp_log_path;
	
	char* tmp_prefix_help = (char*) malloc(sizeof(char*) * (strlen(cfg.prefix_help)+1));
	strcpy(tmp_prefix_help, cfg.prefix_help);
	ret = expand_vars(&tmp_prefix_help, &tmp_nvlist);
	cfg.prefix_help = tmp_prefix_help;
	
	char* tmp_prefix_cmd = (char*) malloc(sizeof(char*) * (strlen(cfg.prefix_cmd)+1));
	strcpy(tmp_prefix_cmd, cfg.prefix_cmd);
	ret = expand_vars(&tmp_prefix_cmd, &tmp_nvlist);
	cfg.prefix_cmd = tmp_prefix_cmd;
	
	// validate and convert values
	int log_length = LOG_LENGTH;
	if (strcmp(cfg.max_log_size_bytes, "") != 0) {
		log_length = atoi(cfg.max_log_size_bytes);
		if (log_length <= 1024) {
			log_length = LOG_LENGTH;
			fprintf(stderr, "invalid value max_log_size_bytes in configuration, using default: %d\n", LOG_LENGTH);
		}
	}
	
	if (strcmp(cfg.log_level, "") != 0) {
		loglevel = atoi(cfg.log_level);
		char buffer[50] = {0};
		strcat(buffer, my_itoa(loglevel));
		if (strcmp(buffer, cfg.log_level) != 0 || loglevel < 0) {
			loglevel = 1;
			fprintf(stderr, "invalid value log_level in configuration, using default: 1\n");
		}
	}
	
	/** 
	 * open log file. 
	 *
	 * use ini value.
	 *
	 * if unset use the platform specific defaults. we check 
	 * for $APPDATA on windows and $HOME on other platforms (env variables).
	 */
	if (strcmp(tmp_log_path, "") == 0) {
		if (getenv("APPDATA") != NULL) {
			strcpy(log_file, getenv("APPDATA"));
			strcat(log_file, "\\");
			strcat(log_file, str(PROGNAME_SHORT));
			strcat(log_file, ".log");
		} else if (getenv("HOME") != NULL) {
			strcpy(log_file, getenv("HOME"));
			strcat(log_file, "/.");
			strcat(log_file, str(PROGNAME_SHORT));
			strcat(log_file, ".log");
		} else
			return display_error(ERR_NO_USERDIR);
	}
	
	// from here on we have a log file we can log to
	// if (loglevel > 0) { // FIXME: replace all code that accesses log_file with writelog() before not opening he file on loglevel 0
		logfile = fopen(log_file, "ab+");
		if (logfile == NULL)
			return display_error(FAILED_TO_OPEN_LOGFILE);
	//}
	
	/**
	 * start logging
	 *
	 * we start with logging our startup information like paths, timestamp, etc.
	 */
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	sprintf(logbuffer, "\r\n===> %d-%02d-%02d %02d:%02d:%02d, URI: '%s'", 
	        tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, 
			tm.tm_min, tm.tm_sec,  argv[1]);
	writelog(1, logbuffer);
	
	if (loglevel > 2) {
		sprintf(logbuffer, "section: %s", cfg.section); 
		writelog(3, logbuffer);
		sprintf(logbuffer, "log_path: %s", cfg.log_path); 
		writelog(3, logbuffer);
		sprintf(logbuffer, "log_level: %s", cfg.log_level); 
		writelog(3, logbuffer);
		sprintf(logbuffer, "prefix_help: %s", cfg.prefix_help); 
		writelog(3, logbuffer);
		sprintf(logbuffer, "prefix_cmd: %s", cfg.prefix_cmd); 
		writelog(3, logbuffer);
		sprintf(logbuffer, "max_log_size_bytes: %s", cfg.max_log_size_bytes); 
		writelog(3, logbuffer);
		sprintf(logbuffer, "found: %d", cfg.found); 
		writelog(3, logbuffer);
	}
	sprintf(logbuffer, "Current working dir: %s", dir);
	writelog(1, logbuffer);
	if (remote_config)
		sprintf(logbuffer, "Current ini file:    %s", argv[2]);
	else
		sprintf(logbuffer, "Current ini file:    %s", ini_file);
	writelog(1, logbuffer);
	
	// display help
	if (argc > 1 && 
	    (strcmp(argv[1], "-h") == 0 || 
		 strcmp(argv[1], "--help") == 0 || 
		 strcmp(argv[1], "/h") == 0 || 
		 strcmp(argv[1], "/H") == 0 || 
		 strcmp(argv[1], "/?") == 0
		)) {
		usage();
		return OK;
	}
	
	// check input
	if(argc < 2) {
		perror("argument 1 with uri missing");
		usage();
		return display_error(NO_INPUT);
	}
	
	/**
	 * parse uri
	 *
	 * The uri passed in as argv[1] is broken down into the following 
	 * components:
	 * - scheme
	 * - authority
	 * - path
	 * - query
	 * - fragment
	 *
	 * ie.: scheme://authority/path/?query#fragment
	 */
	struct t_uri uri_parsed = uriparse_create(argv[1]);
	res = uriparse_parse(argv[1], &uri_parsed);
	//printf("res: %d\n", res);
	if (res != 0) {
		ret = 127+res;
		const char *err;
		if (res == FOUND_PROTO)
			err = "Failed to find protocol delimiter ':'";
		else if (res == FOUND_AUTHORITY)
			err = "Failed to extract authority";
		else if (res == FOUND_PATH)
			err = "Failed to find the path";
		else if (res == FOUND_QUERY)
			err = "Failed to parse the query";
		else if (res == FOUND_FRAGMENT)
			err = "Failed to parse the fragment";
		else if (res == FOUND_END)
			err = "Failed to find the end";

		sprintf(logbuffer, "URI Parser Error %d, %s, %s", res, err, argv[1]);
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
		
		if (loglevel > 3) {
			for(i=0; i<uri_parsed.nvquery.length; i++) {
				sprintf(logbuffer, "[%d] '%s'='%s'", i, uri_parsed.nvquery.items[i].key, uri_parsed.nvquery.items[i].value);
				writelog(4, logbuffer);
			}
		}
	}
	
	/**
	 * parse ini file
	 *
	 * Lookup the ini file. Use authority+path to find a corresponding section
	 * in the ini file.
	 *
	 * if the following uri was called `ph://someapp/path?param=3`, then we'll
	 * check if we can find a section called `[someapp/path]` in the ini file.
	 * On succes, we'll use it's configuration. On failure, we'll abort any 
	 * further processing.
	 */
	l = strlen(uri_parsed.authority) + strlen(uri_parsed.path) + 2;
	char *section = (char *) malloc(sizeof(char*) * l);
	section[0] = 0;
	strcat(section, uri_parsed.authority);
	if (strcmp(uri_parsed.path, "") != 0) {
		strcat(section, "/");
		strcat(section, uri_parsed.path);
	}

	sprintf(logbuffer, "Reading ini section: [%s]", section);
	writelog(1, logbuffer);
	
	// read the config 
	configuration config = DEFAULT_CONFIG;
	config.section = section;
	retp = ini_parse_string(ini_content, ini_callback, &config);
	
	sprintf(logbuffer, "ini_parse() return code: %d", retp);
	writelog(2, logbuffer);
	
	if (retp != 0) {
		return display_error(INI_PARSE_ERR);
	}
	
	if (loglevel > 2) {
		sprintf(logbuffer, "section: %s", config.section); 
		writelog(3, logbuffer);
		sprintf(logbuffer, "default_path: %s", config.default_path); 
		writelog(3, logbuffer);
		sprintf(logbuffer, "allowed_params: %s", config.allowed_params); 
		writelog(3, logbuffer);
		sprintf(logbuffer, "path_params: %s", config.path_params); 
		writelog(3, logbuffer);
		sprintf(logbuffer, "replace_file: %s", config.replace_file); 
		writelog(3, logbuffer);
		sprintf(logbuffer, "replace_regex: %s", config.replace_regex); 
		writelog(3, logbuffer);
		sprintf(logbuffer, "cmd: %s", config.cmd); 
		writelog(3, logbuffer);
		sprintf(logbuffer, "found: %d", config.found); 
		writelog(3, logbuffer);
		
		if (loglevel > 3) {
			sprintf(logbuffer, "lpadzero: %d", config.lpadzero->length); 
			writelog(4, logbuffer);
			
			if (config.lpadzero != 0) {
				for(i=0; i<config.lpadzero->length; i++) {
					sprintf(logbuffer, "[%d] '%s'", i, config.lpadzero->items[i]);
					writelog(4, logbuffer);
				}
			} else {
				sprintf(logbuffer, "NULL");
				writelog(4, logbuffer);
			}
			
			sprintf(logbuffer, "ltrimzero: %d", config.ltrimzero->length); 
			writelog(4, logbuffer);
			
			if (config.ltrimzero != 0) {
				for(i=0; i<config.ltrimzero->length; i++) {
					sprintf(logbuffer, "[%d] '%s'", i, config.ltrimzero->items[i]);
					writelog(4, logbuffer);
				}
			} else {
				sprintf(logbuffer, "NULL");
				writelog(4, logbuffer);
			}
			
		}

	}
	
	if (config.found == 0) {
		sprintf(logbuffer, "Ini section '%s' not found ", section);
		writelog(1, logbuffer);
		fprintf(stderr, "%s\n", logbuffer);
		return display_error(NO_INI_SECTION_FOUND);
	}
	
	if (strcmp(config.cmd, "") == 0)
		return display_error(NO_CMD_DIRECTIVE);
	
	/**
	 * expand variables
	 *
	 * The ini file might use variables in the form of ${varname} in some 
	 * values. There correspond to query parameters passed in in the original 
	 * uri (argv[1]). 
	 *
	 * Also, environment variables are substituted (eg. ${env.HOME}).
	 *
	 */
	char* default_path = (char*) malloc(sizeof(char*) * (strlen(config.default_path)+1));
	strcpy(default_path, config.default_path);
	ret = expand_vars(&default_path, &uri_parsed.nvquery);
	if (ret != 0) {
		return display_error(FAILED_TO_EXPAND_ENV);
	}
	config.default_path = default_path;
	if (loglevel > 1) {
		sprintf(logbuffer, "default_path expanded: %s", config.default_path); 
		writelog(2, logbuffer);
	}
	
	char* replace_file = (char*) malloc(sizeof(char*) * (strlen(config.replace_file)+1));
	strcpy(replace_file, config.replace_file);
	ret = expand_vars(&replace_file, &uri_parsed.nvquery);
	if (ret != 0) {
		return display_error(FAILED_TO_EXPAND_ENV);
	}
	config.replace_file = replace_file;
	if (loglevel > 1) {
		sprintf(logbuffer, "replace_file expanded: %s", config.replace_file); 
		writelog(2, logbuffer);
	}
	
	/** 
	 * do file content replacement
	 *
	 * This application can do manipulations on tect files. if the config 
	 * parameters replace_file and replace_regex are present, the regex will 
	 * be run against the file.
	 * 
	 * This can be useful if you need to switch (for example) host names before 
	 * launching certain programs in certain configurations.
	 */
	if (strcmp(config.replace_file, "") != 0) {
		if (loglevel > 1) {
			sprintf(logbuffer, "Regex replace in: %s", config.replace_file); 
			writelog(2, logbuffer);
			sprintf(logbuffer, "Regex : %s", config.replace_regex); 
			writelog(2, logbuffer);
		}
		if (strcmp(config.replace_regex, "") == 0) {
			sprintf(logbuffer, "Missing regex.");
			writelog(1, logbuffer);
			fprintf(stderr, "%s\n", logbuffer);
			return display_error(MISSING_REGEX);
		}
		
		cmdunquote((char**) &config.replace_file);
		ret = replace(config.replace_file, config.replace_regex);
		
		if (ret != 0) {
			sprintf(logbuffer, "Error regex replacing %s in %s.", 
			        config.replace_file, config.replace_regex);
			writelog(1, logbuffer);
			fprintf(stderr, "%s\n", logbuffer);
			return display_error(ret);
		}
	}
	
	// run transformers on url parameters
	if (config.lpadzero != 0 && config.lpadzero->length > 0) {
		for(i=0; i<uri_parsed.nvquery.length; i++) {
			//sprintf(logbuffer, "[%d] '%s'='%s'", i, uri_parsed.nvquery.items[i].key, uri_parsed.nvquery.items[i].value);
			//writelog(4, logbuffer);
			
			for(ii=0; ii<config.lpadzero->length; ii++) {
				//printf("%s, %s\n", config.lpadzero->items[ii], uri_parsed.nvquery.items[i].key);
				if (strcmp(config.lpadzero->items[ii], uri_parsed.nvquery.items[i].key) != 0)
					continue;
				
				char out[10] = "";
				int r = transform_lpadzero(uri_parsed.nvquery.items[i].value, out, 9);
				if (r != 0) {
					sprintf(logbuffer, "Error while zeropadding %s, too long.", 
							uri_parsed.nvquery.items[i].value);
					writelog(1, logbuffer);
					fprintf(stderr, "%s\n", logbuffer);
					return display_error(FAILED_TO_ZEROPAD);
				}
				uri_parsed.nvquery.items[i].value = out;
			}
		}
	}
	
	if (config.ltrimzero != 0 && config.ltrimzero->length > 0) {
		for(i=0; i<uri_parsed.nvquery.length; i++) {
			//sprintf(logbuffer, "[%d] '%s'='%s'", i, uri_parsed.nvquery.items[i].key, uri_parsed.nvquery.items[i].value);
			//writelog(4, logbuffer);
			
			for(ii=0; ii<config.ltrimzero->length; ii++) {
				//printf("%s, %s\n", config.ltrimzero->items[ii], uri_parsed.nvquery.items[i].key);
				if (strcmp(config.ltrimzero->items[ii], uri_parsed.nvquery.items[i].key) != 0)
					continue;
				
				char out[10] = "";
				transform_ltrimzero(uri_parsed.nvquery.items[i].value, out);
				//printf("out: %s\n", out);
				uri_parsed.nvquery.items[i].value = out;
			}
		}
	}
	
	
	
	// expand variables on cmd
	char* cmd = (char*) malloc(sizeof(char*) * (strlen(config.cmd)+1));
	strcpy(cmd, config.cmd);
	ret = expand_vars(&cmd, &uri_parsed.nvquery);
	
	// FIXME: add information in error message which variable failed to expand.
	if (ret != 0) {
		//printf("Error parsing cmd: %d\n", ret);
		sprintf(logbuffer, "%s, varname=%s", errstr[ret+32], expandvar_err_var_name);
		writelog(1, logbuffer);
		return display_error(ret+32);
	}
	if (loglevel > 1) {
		sprintf(logbuffer, "cmd expanded: %s", cmd); 
		writelog(2, logbuffer);
	}
	
	/**
	 * check if the executable is available on the file system and if it is 
	 * actually executable by the user.
	 */
	char* execfile;
	getexe(cmd, &execfile);
	struct stat sb;
	if (stat(execfile, &sb) == 0 && sb.st_mode & S_IXUSR) {
		sprintf(logbuffer, "Executable file: '%s'", execfile);
		writelog(2, logbuffer);
	} else {
		sprintf(logbuffer, "Not executable: '%s'", execfile);
		writelog(1, logbuffer);
		return display_error(NOT_EXECUTABLE);
	}
	
	/**
	 * create the command
	 *
	 * create a command line object that can be passed to spawnve. We always
	 * run cmd.exe /c to make sure the command is run in a clean environment.
	 *
	 * We never wait for the program to exit. The exit code of the run 
	 * program will therefore never be logged. This is a design decision because
	 * it will be unclear if a non zero exit code is cuase by unclean start 
	 * or later on due to user action or a bug in the called program.
	 */
#ifdef __MINGW32__
	char exe[4096] = "";
	strcat(exe, getenv("windir"));
	strcat(exe, "\\System32\\cmd.exe");

	char* myargs[3];
	myargs[0] = (char*) "/C";
	myargs[1] = (char*) cmd;
	myargs[2] = NULL;
	quote(&myargs[1]);
	
	// FIXME: for osx, use posix_spawn:
	// https://developer.apple.com/legacy/library/documentation/Darwin/Reference/ManPages/man2/posix_spawn.2.html
	ret = spawnve(P_DETACH, exe, myargs, environ);
	if (ret < 0) {
		sprintf(logbuffer, "spawnve() returned error: %d", ret);
		writelog(1, logbuffer);
		fprintf(stderr, "%s\n", logbuffer);
	}
	
	sprintf(logbuffer, "Success: %s /C %s", exe, myargs[1]);
	writelog(1, logbuffer);
	
#else
	// Here be dragons
	char exe[4096] = "/usr/bin/env sh -c '";
	strcat(exe, cmd);
	strcat(exe, "' &");
	/*
	char* myargs[5];
	//myargs[0] = (char*) "sh";
	myargs[0] = (char*) "-c";
	myargs[1] = (char*) cmd;
	myargs[2] = (char*) 0;
	//quote2(&myargs[1], '\'');
	
	printf("command: /bin/sh ");
	for (i=0; i<2; i++) {
		printf("%s ", myargs[i]);
	}
	printf("\n");
	
	pid_t pid;
	int status;
	status = posix_spawn(&pid, "/bin/sh", NULL, NULL, myargs, environ);
	if (status == 0) {
		printf("Child pid: %i\n", pid);
		if (waitpid(pid, &status, 0) != -1) {
			printf("Child exited with status %i\n", status);
		} else {
			perror("waitpid");
		}
	} else {
		printf("posix_spawn: %s\n", strerror(status));
	}
	*/
	system(exe);
	printf("exe: %s\n", exe);
#endif
	
	// skip log shortening if we do not log
	if (loglevel == 0)
		return OK;
	
	fseek(logfile, 0, SEEK_END);
	fsize = ftell(logfile);
	fseek(logfile, 0, SEEK_SET);  //same as rewind(f);

	if (loglevel > 2) {
		sprintf(logbuffer, "logfile size: %ld", fsize); 
		writelog(3, logbuffer);
	}
	
	/**
	 * log cleaning
	 *
	 * keep the logfile at a certain size configured in the ini file.
	 *
	 * whenever the size is exceeded, we remove some log entries fro mthe 
	 * beginning of the file (oldest) to make room for new ones.
	 */
	long newlen = (log_length-1024);
	if (fsize > log_length) {
		if (loglevel > 2) {
			sprintf(logbuffer, "Truncating logfile");
			writelog(3, logbuffer);
		}
		fclose(logfile);
		FILE* logfile = fopen(log_file, "rb");

		fseek(logfile, -1*newlen, SEEK_END);
		char tempchar, tempchar1, tempchar2, tempchar3;
		
		while((tempchar = fgetc(logfile))) {
			newlen--;
			if (tempchar == '=') {
				tempchar1 = fgetc(logfile);
				if (tempchar1 != '=')
					continue;
				tempchar2 = fgetc(logfile);
				if (tempchar2 != '=')
					continue;
				tempchar3 = fgetc(logfile);
				if (tempchar3 != '>')
					continue;
				
				fseek(logfile, -3, SEEK_CUR);
				newlen += 3;
				
				char *string = (char*) malloc(newlen + 1);
				if (string == NULL)
					return FAILED_TO_ALLOC_MEM_FOR_FILE;
				
				//printf("Reading %ld bytes, str len: %llu\n", newlen, strlen(string));
				fread(string, newlen-3, 1, logfile);
				string[newlen-3] = 0;
				fclose(logfile);
				
				//printf("Opening: %s\n", tmplog);
				FILE* fp = fopen(log_file, "wb");
				fwrite(string, 1, strlen(string), fp);
				fclose(fp);
				
				break;
			}
		}
		
	} // end log truncation
	
	return OK;
}
#endif // PH_NO_MAIN

#define MATCH(s, n) strcmp_lcase((char*)section, (char*)s) == 0 && strcmp_lcase((char*)name, (char*)n) == 0
/**
 * callback url directives
 * 
 * [exe/replace]
 * exe = "${USERPROFILE}\Projects\protohand\testcmd.exe"
 * replace_file = ${USERPROFILE}\Projects\protohand\test\textreplacement.txt
 * replace_regex = /e/E/
 */
static int ini_callback(void* user, const char* section, const char* name,
                  const char* value) {
	//static char prev_section[50] = "";
	configuration* pconfig = (configuration*)user;

	if (strcmp_lcase((char*) pconfig->section, (char*) section) == 0) {
		//fprintf(stdout, "%s => %s: %s [%s]\n", section, name, value, pconfig->section);
		sprintf(logbuffer, "%s => %s: %s [%s]\n", section, name, value, pconfig->section);
		writelog(3, logbuffer);

		if (MATCH(section, "default_path")) {
			pconfig->default_path = strdup(value);
			pconfig->found = 1;
		} else if (MATCH(section, "allowed_params")) {
			pconfig->allowed_params = strdup(value);
			pconfig->found = 1;
		} else if (MATCH(section, "path_params")) {
			pconfig->path_params = strdup(value);
			pconfig->found = 1;
		} else if (MATCH(section, "cmd")) {
			pconfig->cmd = strdup(value);
			pconfig->found = 1;
		} else if (MATCH(section, "replace_file")) {
			pconfig->replace_file = strdup(value);
			pconfig->found = 1;
		} else if (MATCH(section, "replace_regex")) {
			pconfig->replace_regex = strdup(value);
			pconfig->found = 1;
			
		} else if (MATCH(section, "ltrimzero")) {
			pconfig->ltrimzero = (str_array*) malloc(sizeof(struct str_array));
			str_array_split_p(pconfig->ltrimzero, (char*) value, (char*) ",");
			pconfig->found = 1;
			
		} else if (MATCH(section, "lpadzero")) {
			pconfig->lpadzero = (str_array*) malloc(sizeof(struct str_array));
			str_array_split_p(pconfig->lpadzero, (char*) value, (char*) ",");
			pconfig->found = 1;
			
		} else {
			sprintf(logbuffer, "Found unknown directive '%s' with value '%s' "
			                   "in ini file.", name, value); 
			writelog(1, logbuffer);
			fprintf(stderr, "%s\n", logbuffer);
			return 0;
		}
		
	}
	// printf("search section: %s\n", (const char *) pconfig->section);

	return 1;
}


/**
 * callback for the global config
 * 
 * [_golbal]
 * log_path = ${env.APPDATA}\ph.log
 * log_level = 1
 * prefix_help = ${env.windir}\cmd.exe /C hh.exe -800
 * prefix_cmd = ${env.windir}\cmd.exe /C
 * max_log_size_bytes = 10240000
 */
static int global_callback(void* user, const char* section, const char* name,
                  const char* value) {
	//static char prev_section[50] = "";
	global_configuration* pconfig = (global_configuration*)user;

	//#define MATCH(s, n) strcmp_lcase((char*)section, (char*)s) == 0 && strcmp_lcase((char*)name, (char*)n) == 0
	//printf("section: %d '%s' = '%s'\n", strcmp(pconfig->section, section), section, pconfig->section);
	if (strcmp_lcase((char*) pconfig->section, (char*) section) == 0) {
		sprintf(logbuffer, "%s => %s: %s [%s]\n", section, name, value, pconfig->section);
		writelog(3, logbuffer);

		if (MATCH(section, "log_path")) {
			pconfig->log_path = strdup(value);
			pconfig->found = 1;
		} else if (MATCH(section, "log_level")) {
			pconfig->log_level = strdup(value);
			pconfig->found = 1;
		} else if (MATCH(section, "prefix_help")) {
			pconfig->prefix_help = strdup(value);
			pconfig->found = 1;
		} else if (MATCH(section, "prefix_cmd")) {
			pconfig->prefix_cmd = strdup(value);
			pconfig->found = 1;
		} else if (MATCH(section, "max_log_size_bytes")) {
			pconfig->max_log_size_bytes = strdup(value);
			pconfig->found = 1;
		} else {
			sprintf(logbuffer, "Found unknown directive '%s' with value '%s' "
			                   "in ini file.", name, value); 
			writelog(1, logbuffer);
			fprintf(stderr, "%s\n", logbuffer);
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
			sprintf(logbuffer, "Failed to created ini file '%s'. "
							   "Permission denied.", ini_file);
			writelog(1, logbuffer);

			return INI_PERMISSION_DENIED;
		}
		
		sprintf(logbuffer, "Failed to created ini file '%s'. "
						 "Error: %d: %s.", ini_file, errno, 
						 strerror(errno));
		writelog(1, logbuffer);
		
		perror("Failed to create ini file.");
		return INI_CREATION_FAILED;
	}
	
	// write default ini file 
	//printf("length: %d\n", sizeof(ini_str));
	fwrite(ini_str , sizeof(char), strlen(ini_str), fpinifile);
	fclose(fpinifile);
	
	FILE* fpreadme;
	fpreadme = fopen("README.txt", "wb+");
	fwrite(usage_str , sizeof(char), strlen(usage_str), fpreadme);
	fclose(fpreadme);

#ifdef __MINGW32__
	FILE* fpreg;
	fpreg = fopen("ph.reg", "wb+");
	fwrite(reg_str , sizeof(char), strlen(reg_str), fpreg);
	fclose(fpreg);
#endif

	FILE* fperror;
	fperror = fopen("error.html", "wb+");
	fwrite(error_str , sizeof(char), strlen(error_str), fperror);
	fclose(fperror);

	
	// show user that the ini file was created and quit
	printf("Example ini file has been created. You should review and "
		   "change it to your needs. Aborting.");
	return OK;
}
