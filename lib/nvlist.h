#ifndef __NVLIST_H__
#define __NVLIST_H__

/* Make this header file easier to include in C++ code */
#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../lib/stringlib.h"

struct nvlist_pair {
	char *key;
	char* value;
};

struct nvlist_list {
	struct nvlist_pair *items;
	int length;
	int max;
	int _step;
};

int nvlist_addpair(struct nvlist_list *rep, char* key, char* value);
int nvlist_addstr(struct nvlist_list *rep, char* str, char delim);
int nvlist_resize(struct nvlist_list* rep, int size);
struct nvlist_list nvlist_create(int size);
void nvlist_destroy(struct nvlist_list *rep);
int nvlist_find_parts(char* string, char search);

#ifdef __cplusplus
}
#endif

#endif /* __NVLIST_H__ */