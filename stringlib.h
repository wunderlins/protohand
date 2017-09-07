#ifndef __STRINGLIB_H__
#define __STRINGLIB_H__

/* Make this header file easier to include in C++ code */
#ifdef __cplusplus
extern "C" {
#endif

char *trim(char *str);
int split(char* string, char* delim, char** result, int *count);
int findchar(const char* allowed, const char* string);
int find_param(char* search, char** options);

#ifdef __cplusplus
}
#endif

#endif /* __STRINGLIB_H__ */
