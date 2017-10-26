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
int loglevel = 5; // 0 will disable logging
char logbuffer[4096];
char log_file[MAX_CWD_LENGTH];

char encodek[20];

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
	char params[MAX_CWD_LENGTH];
	
	sprintf(params, "%s/error.html?%d", getenv("PH_HOME"), code);
	const char* myargs[5] = {
		"/c"
		"hh.exe",
		"-800",
		params,
		NULL
	};
	char exe[4096] = "";
	strcat(exe, getenv("windir"));
	strcat(exe, "\\System32\\cmd.exe");
	spawnve(P_NOWAIT, exe, (char* const*) myargs, (char* const*) environ);
	
	//define_error_messages();
	//fprintf(stderr, "%s\n", errstr[code]);
	fprintf(stderr, "Error: %d, %s\n", code, errstr[code]);
	sprintf(logbuffer, "Error: %d, %s", code, errstr[code]);
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
	transcode_str(string, &fsize, &result, encodek);
	
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
	environ = envp;
	
	int i = 0;
	//int ii = 0;
	int l = 0;
	int ret = 0;
	int res;

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
	encodek[19] = 'd';
	
	define_error_messages();
	
	// find the current directory of the executable
	char *dir = (char*) malloc(sizeof(char*) * (MAX_CWD_LENGTH+1));
	int r = exedir(argv[0], dir);
	if (r != 0) {
		fprintf(stderr, "Failed to find current directory, error: %d\n", r);
		return display_error(NO_CURRENTDIR);
	}
	
	// set an environment Variable with our directroy path
	char env[strlen(dir)+9] = {0};
	strcat(env, "PH_HOME=");
	strcat(env, dir);
	env[strlen(env)-1] = 0;
	putenv(env);
	
	// open log file
	strcpy(log_file, dir);
	strcat(log_file, "ph.log");
	logfile = fopen(log_file, "ab+");
	
	// logging uri
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	sprintf(logbuffer, "\r\n===> %d-%02d-%02d %02d:%02d:%02d, URI: '%s'", 
	        tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, 
			tm.tm_min, tm.tm_sec,  argv[1]);
	writelog(1, logbuffer);

	sprintf(logbuffer, "Current working dir: %s", dir);
	writelog(2, logbuffer);
	
	// use ini file from argv or in the exe folder ?
	char ini_file[MAX_CWD_LENGTH];
	if (argc == 3) {
		strcpy(ini_file, argv[2]);
	} else {
		strcpy(ini_file, dir);
		strcat(ini_file, INI_FILE_NAME);
	}
	
	sprintf(logbuffer, "Current ini file:    %s", ini_file);
	writelog(2, logbuffer);
	

	// check command line parameters for mode
	// encode config file
	// PROGNAME_SHORT_EXT -e <config_file> [out_file]
	if (strcmp(argv[1], "-e") == 0)
		return encode_file(argc, argv);
	
	// test regex
	// PROGNAME_SHORT_EXT -r </regex/replace/> <file>
	if (strcmp(argv[1], "-r") == 0)
		return test_regex(argc, argv);

	// check if we have an ini file, if not, create it
	if( access(ini_file, F_OK) == -1 ) {
		// file doesn't exist, create it
		ret = create_ini(ini_file);
		return OK;
	}
	
	// check input
	if(argc < 2) {
		perror("argument 1 with uri missing");
		usage();
		return display_error(NO_INPUT);
	}

	
	// parse uri
	//struct t_uri uri_parsed = {uri, empty, empty, empty, empty, empty, {-1, -1, -1, -1, -1, -1, -1}};
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
	}
	
	if (config.found == 0) {
		sprintf(logbuffer, "Ini section '%s' not found ", section);
		writelog(1, logbuffer);
		fprintf(stderr, "%s\n", logbuffer);
		return display_error(NO_INI_SECTION_FOUND);
	}
	
	if (strcmp(config.cmd, "") == 0)
		return display_error(NO_CMD_DIRECTIVE);
	
	// expand environment variables
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
	
	// do file content replacement
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
			return MISSING_REGEX;
		}
		
		// FIXME: check if we find a sane way to handle quotes in filenames
		cmdunquote((char**) &config.replace_file);
		ret = replace(config.replace_file, config.replace_regex);
		
		if (ret != 0) {
			sprintf(logbuffer, "Error regex replacing %s in %s.", 
			        config.replace_file, config.replace_regex);
			writelog(1, logbuffer);
			fprintf(stderr, "%s\n", logbuffer);
			// FIXME: use display_error
			return ret;
		}
	}
	
	// expand variables
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
	
	// TODO: check path parameters
	// TODO: run command, might have to split it
	//printf("out: %s\n", cmd);
	
	char exe[4096] = "";
	strcat(exe, getenv("windir"));
	strcat(exe, "\\System32\\cmd.exe");

	char* myargs[3];
	myargs[0] = (char*) "/c";
	myargs[1] = (char*) cmd;
	myargs[2] = NULL;
	
	// FIXME: quoted paths with spaces are not run by 
	//         cmd.exe
	quote(&myargs[1]);
	//printf("cmd: %s\n", myargs[1]);
	// FIXME: for osx, ue posix_spawn:
	// https://developer.apple.com/legacy/library/documentation/Darwin/Reference/ManPages/man2/posix_spawn.2.html
	ret = spawnve(P_NOWAIT, exe, myargs, environ);
	if (ret < 0) {
		sprintf(logbuffer, "spawnve() returned error: %d", ret);
		writelog(1, logbuffer);
		fprintf(stderr, "%s\n", logbuffer);
	}
	
	sprintf(logbuffer, "Success: %s /c %s", exe, myargs[1]);
	writelog(1, logbuffer);
	
	fseek(logfile, 0, SEEK_END);
	long fsize = ftell(logfile);
	fseek(logfile, 0, SEEK_SET);  //same as rewind(f);

	if (loglevel > 2) {
		sprintf(logbuffer, "logfile size: %ld", fsize); 
		writelog(3, logbuffer);
	}

	long newlen = (LOG_LENGTH-1024);
	
	if (fsize > LOG_LENGTH) {
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
		} else if (MATCH(section, "cmd")) {
			pconfig->cmd = strdup(value);
			pconfig->found = 1;
		} else if (MATCH(section, "replace_file")) {
			pconfig->replace_file = strdup(value);
			pconfig->found = 1;
		} else if (MATCH(section, "replace_regex")) {
			pconfig->replace_regex = strdup(value);
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

	FILE* fpreg;
	fpreg = fopen("ph.reg", "wb+");
	fwrite(reg_str , sizeof(char), strlen(reg_str), fpreg);
	fclose(fpreg);
	
	FILE* fperror;
	fperror = fopen("error.html", "wb+");
	fwrite(error_str , sizeof(char), strlen(error_str), fperror);
	fclose(fperror);

	
	// show user that the ini file was created and quit
	printf("Example ini file has been created. You should review and "
		   "change it to your needs. Aborting.");
	return OK;
}