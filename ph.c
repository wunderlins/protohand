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
	//printf("spawnv %d\n", ret);
	
	// TODO: return message to stderr
	
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

#ifndef PH_NO_MAIN
//char **environ;
int main(int argc, char** argv, char **envp) {
	environ = envp;
	
	int i = 0;
	int ii = 0;
	int l = 0;
	int ret = 0;
	int res;
	
	/*
	// cmd parser test
	char* uri = (char*) "proto:auth/path?name1=vaue1&name2=wus";
	char* cmd = (char*) "${env.HOME}\\notepad.exe /A \"${name1}\" ${name2} ${ env.USERNAME != name2 : --debug }";
	struct t_uri urit = uriparse_create(uri);
	
	urit = uriparse_create(uri);
	res = uriparse_parse(uri, &urit);
	//printf("res: %d\n", res);
	if (res != 0) {
		ret = 127+res;
		printf("Parser Error %d, %s\n", res, uri);
		return ret;
	}
	
	ret = expand_vars(&cmd, &urit.nvquery);
	printf("out: %s\n", cmd);
	return 0;
	*/
	
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
	putenv(env);
	
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
	//struct t_uri uri_parsed = {uri, empty, empty, empty, empty, empty, {-1, -1, -1, -1, -1, -1, -1}};
	struct t_uri uri_parsed = uriparse_create(argv[1]);
	res = uriparse_parse(argv[1], &uri_parsed);
	if (res != 0) {
		ret = 127+res;
		const char *err;
		if (ret == FOUND_PROTO)
			err = "Failed to find protocol delimiter ':'";
		else if (ret == FOUND_AUTHORITY)
			err = "Failed to extract authority";
		else if (ret == FOUND_PATH)
			err = "Failed to find the path";
		else if (ret == FOUND_QUERY)
			err = "Failed to parse the query";
		else if (ret == FOUND_FRAGMENT)
			err = "Failed to parse the fragment";
		else if (ret == FOUND_END)
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
		sprintf(logbuffer, "params_prepend: %s", config.params_prepend); 
		writelog(3, logbuffer);
		sprintf(logbuffer, "params_append: %s", config.params_append); 
		writelog(3, logbuffer);
		sprintf(logbuffer, "replace_file: %s", config.replace_file); 
		writelog(3, logbuffer);
		sprintf(logbuffer, "replace_regex: %s", config.replace_regex); 
		writelog(3, logbuffer);
		sprintf(logbuffer, "exe: %s", config.exe); 
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
	
	// expand environment variables
	ret = expenv((char**) &config.exe);
	if (ret != 0) {
		return display_error(FAILED_TO_EXPAND_ENV);;
	}

	ret = expenv((char**) &config.default_path);
	if (ret != 0) {
		return display_error(FAILED_TO_EXPAND_ENV);;
	}

	ret = expenv((char**) &config.replace_file);
	if (ret != 0) {
		return display_error(FAILED_TO_EXPAND_ENV);;
	}
	
	struct {
		int exe;
		int default_path;
	} quoted = {0, 0};
	
	if (isquoted((char*) config.exe) == '"') {
		//char *newexe = (char*) malloc(sizeof(char*) * (strlen(config.exe)+1));
		//strcpy(newexe, config.exe);
		//config.exe = newexe;
		cmdunquote((char**) &config.exe);
		quoted.exe = 1;
	}
	
	if (isquoted((char*) config.default_path) == '"') {
		cmdunquote((char**) &config.default_path);
		quoted.default_path = 1;
	}
	
	if (config.found == 0) {
		return display_error(INI_NO_SECTION);
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
	//printf("-> exe: %s\n", config.exe);
	//return 0;
	
	// do file content replacement
	if (strcmp(config.replace_file, "") != 0) {
		if (strcmp(config.replace_regex, "") == 0) {
			sprintf(logbuffer, "Missing regex.");
			writelog(1, logbuffer);
			fprintf(stderr, "%s\n", logbuffer);
			return MISSING_REGEX;
		}
		
		cmdunquote((char**) &config.replace_file);
		ret = replace(config.replace_file, config.replace_regex);
		
		if (ret != 0) {
			sprintf(logbuffer, "Error regex replacing %s in %s.", 
			        config.replace_file, config.replace_regex);
			writelog(1, logbuffer);
			fprintf(stderr, "%s\n", logbuffer);
			return ret;
		}
	}
	
	char space[2] = " ";
	// TODO: check env parameters
	
	// replace url parameter names with cmd args
	if (strcmp(config.params_transform, "") != 0) {
		struct str_array pairs = str_array_split((char*) config.params_transform, space);
		struct nvlist_list repl = nvlist_create(10);
		for (i=0; i<pairs.length; i++) {
			nvlist_addstr(&repl, pairs.items[i], '=');
			//printf("pair: %s\n", pairs.items[i]);
		}
		
		for(i=0; i<uri_parsed.nvquery.length; i++) {
			printf("key: %s\n", uri_parsed.nvquery.items[i].key);
			if (strcmp(uri_parsed.nvquery.items[i].key, "") != 0) {
				//printf("query key: %s\n", uri_parsed.nvquery.items[i].key);
				//printf("repl  key: %s\n", repl.items[ii].key);
				for(ii=0; ii<pairs.length; ii++) {
					if (strcmp(repl.items[ii].key, uri_parsed.nvquery.items[i].key) == 0) {
						
						if (loglevel > 2) {
							sprintf(logbuffer, "replacing param '%s' with '%s'", 
							        uri_parsed.nvquery.items[i].key,
									repl.items[ii].value); 
							writelog(3, logbuffer);
						}

						uri_parsed.nvquery.items[i].key = repl.items[ii].value;
						break;
					}
				}
				
			}
		}

	}
	
	// TODO: check if document is within default path
	// TODO: check if values of path parameters are inside default path
	
	// prepare the command line arguments
	struct str_array params_prepend = str_array_split((char*) 
		config.params_prepend, space);
	struct str_array params_append  = str_array_split((char*) 
		config.params_append, space);
	
	// create the arguments array
	// TODO: Document limitation
	char **params = (char**) malloc(sizeof(char*) * MAX_PARAMS);
	int params_length = 0;
	
	//printf("End %d, %s\n", params_prepend.length, params_prepend.items[0]);
	
	// add prepend parameters to params
	if (params_prepend.length) {
		sprintf(logbuffer, "Prepending parameters: %s", config.params_prepend);
		writelog(1, logbuffer);
		
		for(i=0; i<params_prepend.length; i++) {
			if (params_length>MAX_PARAMS) return display_error(TOO_MANY_CMD_ARGUMENTS);
			params[params_length++] = params_prepend.items[i];
		}
		//printf("End\n"); return 0;
	}
	
	// TODO: command line parameter transformation, 'params_transform'
	//       directive.
	
	// check if we have some query parameters
	if (uri_parsed.nvquery.length > 0) {
		for(i=0; i<uri_parsed.nvquery.length; i++) {
			if (params_length>MAX_PARAMS) return display_error(TOO_MANY_CMD_ARGUMENTS);
			
			// name=value ?
			if (strcmp(uri_parsed.nvquery.items[i].key, "") != 0 &&
			    strcmp(uri_parsed.nvquery.items[i].value, "") != 0) {
				//printf("ya\n");
				char *tmp = (char*) malloc(sizeof(char*) * \
					(strlen(uri_parsed.nvquery.items[i].key)+strlen(uri_parsed.nvquery.items[i].value)+2));
				strcpy(tmp, uri_parsed.nvquery.items[i].key);
				strcat(tmp, "=");
				strcat(tmp, uri_parsed.nvquery.items[i].value);
				
				params[params_length++] = tmp;
			} else { // only name or value
				// document link ?
				if (uri_parsed.nvquery.length == 1 && 
					strcmp(uri_parsed.nvquery.items[i].value, "") == 0 &&
				    strcmp(config.default_path, "") != 0) {
					
					char tmp[MAX_CWD_LENGTH] = "";
					strcpy(tmp, config.default_path);
					l = strlen(config.default_path);
					if (config.default_path[l-1] != '/' &&
					    config.default_path[l-1] != '\\')
						strcat(tmp, "/");
					strcat(tmp, uri_parsed.nvquery.items[i].key);
					
					// FIXME: check if file exists, realpath(), jada jada
					params[params_length++] = tmp;
					
				} else { // non document link
					if (strcmp(uri_parsed.nvquery.items[i].key, "") != 0)
						params[params_length++] = uri_parsed.nvquery.items[i].key;
				
					if (strcmp(uri_parsed.nvquery.items[i].value, "") != 0)
						params[params_length++] = uri_parsed.nvquery.items[i].value;
				}
			}
		}
	}
	
	// add append parameters
	if (params_append.length) {
		sprintf(logbuffer, "Appending parameters: %s", config.params_append);
		writelog(1, logbuffer);
		
		for(i=0; i<params_append.length; i++) {
			if (params_length>MAX_PARAMS) return display_error(TOO_MANY_CMD_ARGUMENTS);
			params[params_length++] = params_append.items[i];
		}
	}
	
	// NULL delimit list
	params[params_length] = NULL;
	
	// create params array
	struct str_array aparams = str_array_make(params, params_length);
	
	// #define NUMARGS (3)
	int myargs_length = aparams.length+3;
	char *myargs[myargs_length];
	myargs[0] = (char*) "/c";
	
	// quote exe name
	// FIXME: also check for quoted default_path
	if (quoted.exe) {
		char* tmpexe = (char*) malloc(sizeof(char*) * (strlen(config.exe)+3));
		strcpy(tmpexe, "\"");
		strcat(tmpexe, config.exe);
		strcat(tmpexe, "\"");
		config.exe = tmpexe;
	}
	myargs[1] = (char *) config.exe;
	
	// debug, deplsay parameters
	for(i=0; i<aparams.length; i++) {
		printf("%s\n", aparams.items[i]);
		myargs[i+2] = aparams.items[i];
	}
	myargs[aparams.length+2] = NULL;
	
	char exe[4096] = "";
	strcat(exe, getenv("windir"));
	strcat(exe, "\\System32\\cmd.exe");

	sprintf(logbuffer, "running command: %s", exe);
	for(i=0; i<myargs_length-1; i++) {
		strcat(logbuffer, " ");
		strcat(logbuffer, myargs[i]);
	}
	writelog(1, logbuffer);

	// we run the application in a new process, without waiting for it to exit.
	// this means we can't capture the exit code. Which might be helpful with 
	// startup errors but in other situations this is a useless information
	// because the application can ugracefully exit later on which has nothing 
	// to do with us starting it.
	
	// FIXME: executables with spaces in name do not run when not quoted, 
	//        exe without space in path do not run when quoted! w00t? can be 
	//        reproduced on the cmd.
	ret = spawnve(P_NOWAIT, exe, myargs, environ);
	if (ret < 0) {
		sprintf(logbuffer, "spawnve() returned error: %d", ret);
		writelog(1, logbuffer);
		fprintf(stderr, "%s\n", logbuffer);
	}
	
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
		} else if (MATCH(section, "params_prepend")) {
			pconfig->params_prepend = strdup(value);
			pconfig->found = 1;
		} else if (MATCH(section, "params_append")) {
			pconfig->params_append = strdup(value);
			pconfig->found = 1;
		} else if (MATCH(section, "exe")) {
			pconfig->exe = strdup(value);
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
		} else if (MATCH(section, "params_transform")) {
			pconfig->params_transform = strdup(value);
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