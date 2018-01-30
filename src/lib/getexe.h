#ifndef __GETEXE_H__
#define __GETEXE_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "stringlib.h"

/* Make this header file easier to include in C++ code */
#ifdef __cplusplus
extern "C" {
#endif

int getexe(char* cmd, char** exe);

#ifdef __cplusplus
}
#endif


#endif /* __GETEXE_H__ */
