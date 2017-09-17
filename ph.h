#ifndef __PH_H__
#define __PH_H__

/* Make this header file easier to include in C++ code */
#ifdef __cplusplus
extern "C" {
#endif

// error codes
#define OK 0
#define NO_INPUT 1
#define NO_CURRENTDIR 2
#define INI_PERMISSION_DENIED 3
#define INI_CREATION_FAILED 4

// limits
#define INI_FILE_NAME "ph.ini"
#define MAX_CWD_LENGTH 1024

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include "lib/ini.h"
#include "lib/stringlib.h"
#include "lib/nvlist.h"
#include "lib/uriparse.h"
#include "example_ini.h"
#include "README.h"

static int ini_callback(void* user, const char* section, const char* name, const char* value);
int create_ini(char* ini_file);

#ifdef __cplusplus
}
#endif

#endif /* __PH_H__ */