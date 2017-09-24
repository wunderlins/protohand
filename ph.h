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
#include <process.h> 
#include "lib/ini.h"
#include "lib/stringlib.h"
#include "lib/nvlist.h"
#include "lib/uriparse.h"
#include "example_ini.h"
#include "README.h"
#include "reg.h"
#include "lib/regcpp.h"
#include "lib/mydir.h"

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


/* Make this header file easier to include in C++ code */
#ifdef __cplusplus
extern "C" {
#endif

static int ini_callback(void* user, const char* section, const char* name, const char* value);
int create_ini(char* ini_file);

#ifdef __cplusplus
}
#endif

#endif /* __PH_H__ */