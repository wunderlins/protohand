#ifndef __PH_H__
#define __PH_H__

#ifndef DEBUG
	#define DEBUG 0
#endif

// limits
#define INI_FILE_NAME "ph.ini"
#define MAX_PARAMS 512

// includes
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stddef.h>
#ifdef __APPLE__
	#include <spawn.h>
#else
	#include <process.h>
#endif
#include "lib/ini.h"
#include "lib/stringlib.h"
#include "lib/nvlist.h"
#include "lib/uriparse.h"
#include "lib/regcpp.h"
#include "lib/cmd_parser.h"
#include "lib/xor.h"

#include "generated/example_ini.h"
#include "generated/README.h"
#include "generated/reg.h"
#include "generated/error.h"

#include "lib/regcpp.h"
#include "lib/mydir.h"
#include "lib/errstr.h"

// error codes
#define OK 0
#define NO_INPUT 1
#define NO_CURRENTDIR 2
#define INI_PERMISSION_DENIED 3
#define INI_CREATION_FAILED 4
#define INI_PARSE_ERR 5
#define PROGRAM_IS_NOT_EXECUTABLE 6
#define INI_NO_SECTION 7
#define TOO_MANY_CMD_ARGUMENTS 8

#define FAILED_TO_OPEN_REPLACE_FILE 9
#define FAILED_TO_PARSE_REGEX 10
#define FAILED_TO_ALLOC_MEM_FOR_FILE 11
#define MISSING_REGEX 12

#define NO_INI_SECTION_FOUND 13
#define FAILED_TO_EXPAND_ENV 14
#define NO_CMD_DIRECTIVE 15

#define ERR_LOG_DELETE 16
#define ERR_LOG_RENAME 17

#define PH_EXP_ERR_NO_EQUAL (EXP_ERR_NO_EQUAL+32) 
#define PH_EXP_ERR_NO_COLON (EXP_ERR_NO_COLON+32)
#define PH_EXP_ERR_MALLOC (EXP_ERR_MALLOC+32)
#define PH_EXP_ERR_ENVVAR_NOT_FOUND (EXP_ERR_ENVVAR_NOT_FOUND+32) 
#define PH_EXP_ERR_QUERYNVVAR_NOT_FOUND (EXP_ERR_QUERYNVVAR_NOT_FOUND+32) 
#define PH_EXP_ERR_REALLOC (EXP_ERR_REALLOC+32)

#define LOG_LENGTH (1024*1000*1)

/* Make this header file easier to include in C++ code */
#ifdef __cplusplus
extern "C" {
#endif

extern char* errstr[];
extern char* expandvar_err_var_name;

// can hold one ini file entry
#define DEFAULT_CONFIG { "", "", "", "", "", "", "", 0}
typedef struct {
	const char* section; // the section we are searchin for
	const char* default_path;
	const char* allowed_params;
	const char* path_params;
	const char* replace_file;
	const char* replace_regex;
	const char* cmd;
	int found; // 1 if the section was found. initialize it to 0 otherwise
} configuration;

static int ini_callback(void* user, const char* section, const char* name, const char* value);
int create_ini(char* ini_file);
//void define_error_messages();
void writelog(int level, char* str);
int display_error(int code);

#ifdef __cplusplus
}
#endif

#endif /* __PH_H__ */