#ifndef __CMD_PARSER_H__
#define __CMD_PARSER_H__
/* Make this header file easier to include in C++ code */
#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../lib/stringlib.h"
#include "../lib/nvlist.h"
#include "../lib/uriparse.h"

#define BLOCKSIZE 100

#define EXP_ERR_NO_EQUAL 1 
#define EXP_ERR_NO_COLON 2
#define EXP_ERR_MALLOC 3 
#define EXP_ERR_ENVVAR_NOT_FOUND 4 
#define EXP_ERR_QUERYNVVAR_NOT_FOUND 5 
#define EXP_ERR_REALLOC 6 

#ifdef __cplusplus
}
#endif

#endif // __CMD_PARSER_H__