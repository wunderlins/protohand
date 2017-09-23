#ifndef __MYDIR_H__
#define __MYDIR_H__

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#if defined(_WIN32) || defined(_WIN64) || defined(__CYGWIN__)
    // Windows (x86 or x64)
	#include <limits.h>
	#include "../lib/realpath.h"
#elif defined(__linux__)
    // Linux
    #include <linux/limits.h>
#elif defined(__APPLE__) && defined(__MACH__)
    // Mac OS
    #include <sys/syslimits.h>
#elif defined(unix) || defined(__unix__) || defined(__unix)
    // Unix like OS
	#ifndef PATH_MAX 
    #define PATH_MAX (260)
	#endif
#else
    #error Unknown environment!
#endif
#define MAX_CWD_LENGTH PATH_MAX


/* Make this header file easier to include in C++ code */
#ifdef __cplusplus
extern "C" {
#endif

void fwdslash(char* path);
int exedir(char *argv0, char *exedir);

#ifdef __cplusplus
}
#endif


#endif /* __MYDIR_H__ */