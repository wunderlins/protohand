#ifndef __STRINGLIB_H__
#define __STRINGLIB_H__

/* Make this header file easier to include in C++ code */
#ifdef __cplusplus
extern "C" {
#endif

FILE* logfile;

struct str_array {
    char **items;
    int length;
};

char *trim(char *str);
char *trim2(char *str);
int split(char* string, char* delim, char** result, int *count);
int findchar(const char* allowed, const char* string);
int find_substr(char* string, char* search);
int starts_with(const char *pre, const char *str);
int get_value_from_argument(char* argument, char* ret);

char* find_key(char* search);
int find_param(char* search, struct str_array* options);

#define str_array(elements...) ({ char *values[] = { elements }; str_array_make(values, sizeof(values) / sizeof(char *)); })
#define str_array_destroy(arr) ({ free(arr.items); })
struct str_array str_array_make(char **elements, int count);
struct str_array str_array_split(char* string, char* delim);

int strpos(char* string, char find);
void urldecode2(char*, const char*);

void hex_dump(char *desc, void *addr, int len);

char isquoted(char* str);
int quote(char** str);
int cmdquote(char** str);
int cmdunquote(char** str);


#ifdef __cplusplus
}
#endif

#endif /* __STRINGLIB_H__ */
