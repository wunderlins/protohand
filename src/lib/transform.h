#ifndef __TRANSFORM_H__
#define __TRANSFORM_H__

/* Make this header file easier to include in C++ code */
#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <string.h>
#include "../lib/nvlist.h"

int transform_lpadzero(char* strin, char* strout, int maxlen);
int transform_ltrimzero(char* strin, char* strout);

#ifdef __cplusplus
}
#endif

#endif /* __TRANSFORM_H__ */
