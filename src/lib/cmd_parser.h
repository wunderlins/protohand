#ifndef __CMD_PARSER_H__
#define __CMD_PARSER_H__
/* Make this header file easier to include in C++ code */
#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "../lib/stringlib.h"
#include "../lib/nvlist.h"
#include "../lib/uriparse.h"

// FIXME: BLOCKSIZE should not be that large. This is a temporary because we 
//        believe that memory realocation of the out variable in expand_vars() 
//        leads to a stack error. Needs further investigation.
#define BLOCKSIZE 4096

#define EXP_ERR_NO_EQUAL 1 
#define EXP_ERR_NO_COLON 2
#define EXP_ERR_MALLOC 3 
#define EXP_ERR_ENVVAR_NOT_FOUND 4 
#define EXP_ERR_QUERYNVVAR_NOT_FOUND 5 
#define EXP_ERR_REALLOC 6 

typedef struct {
	char* var1;
	char* var2;
	int sign;
	char* replace;
	int match;
} t_conditional;

int find_var_value(char* varname, struct nvlist_list* query, char** result);
/**
 * parse the conditional
 * schema: var[ \t]*[!]=[ \t]*var[ \t]*:[ \t]*value[ \t]*
 */
int parse_conditional(char* varname, t_conditional* cond, struct nvlist_list* query);
int append_resize(char* string, char* append, int bufflength, int blocksize);
int expand_vars(char** str, struct nvlist_list* query);


#ifdef __cplusplus
}
#endif

#endif // __CMD_PARSER_H__