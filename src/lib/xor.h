#ifndef __XOR_H__
#define __XOR_H__

/* Make this header file easier to include in C++ code */
#ifdef __cplusplus
extern "C" {
#endif

typedef enum {DECODE, ENCODE} xormode_t;

int is_encoded(char* string);
char* _xor(char *string, const char *key, long l);
int transcode_file(char* fin, char* fout, char* key);
int transcode_str(char* strin, long* fsize, char** strout, char* key);

#ifdef __cplusplus
}
#endif

#endif /* __XOR_H__ */