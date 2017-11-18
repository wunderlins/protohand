#ifndef __HTTPGET_H__
#define __HTTPGET_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <curl/curl.h>


/* Make this header file easier to include in C++ code */
#ifdef __cplusplus
extern "C" {
#endif

void tempfilepath(char** tempfile);
int getfile(char* url);

#ifdef __cplusplus
}
#endif


#endif /* __HTTPGET_H__ */