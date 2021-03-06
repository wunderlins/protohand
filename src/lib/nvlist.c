#include "nvlist.h"

#define strmalloc(length) ( \
	malloc(sizeof(char*) * (length+1)) \
)

int nvlist_addpair(struct nvlist_list *rep, char* key, char* value) {
	struct nvlist_pair p;
	p.key = key;
	p.value = value;
	
	if (rep->length+1 > rep->max) {
		//printf("new size %d\n", rep->max+rep->_step);
		int r = nvlist_resize(rep, rep->max+rep->_step);
		if (r < 0)
			return r;
	}
	
	rep->items[rep->length] = p;
	
	//printf("k: '%s', v: '%s'\n", rep->items[rep->length].key, rep->items[rep->length].value);
	
	rep->length++;
	return rep->length;
}

int nvlist_addstr(struct nvlist_list *rep, char* str, char delim) {
	int i = 0;
	int len = strlen(str);
	//printf("len: %d\n", len);
	for(i=0; i < len; i++) {
		//printf("%c\n", str[i]);
		if (str[i] == delim) {
			char *key = malloc(sizeof(char*) * (i+1));
			char *val = malloc(sizeof(char*) * len);
			strncpy(key, str, i);
			key[i] = '\0';
			strncpy(val, str+i+1, len);
			//printf("k %s, v %s\n", key, val);
			return nvlist_addpair(rep, key, val);
		}
	}
	
	return -1;
}

struct nvlist_list nvlist_create(int size) {
	struct nvlist_list rep;
	rep.items = malloc(sizeof(struct nvlist_pair) * size);
	rep.length = 0;
	rep.max = size;
	rep._step = size;
	if (size == 0)
		rep._step = 10;
	return rep;
}

void nvlist_destroy(struct nvlist_list *rep) {
	int i;
	for (i=0; i<rep->length; i++) {
		//printf("%s=%s\n", rep.items[i].key, rep.items[i].value);
		free(rep->items[i].key);
		free(rep->items[i].value);
	}
	
	free(rep->items);
	free(rep);
}

int nvlist_resize(struct nvlist_list* rep, int size) {
	//printf(" -> size: %d, max: %d, length %d\n", size, rep->max, rep->length);
	if (size <= rep->max)
		return -2;
	
	rep->items = realloc(rep->items, sizeof(struct nvlist_pair) * size);
	//printf("realloc ret: %d\n", tmp);
	if (rep->items == NULL)
		return -1;
	
	rep->max = size;
	return rep->max;
}

int nvlist_find_parts(char* string, char search) {
	
	int i, found = 0;
	int len = strlen(string);
	for(i=0; i<len; i++)
		if (string[i] == search)
			found++;
	
	return found+1;
}

#ifdef NVLIST_MAIN
int main(int argc, char** argv, char **envp) {
	
	int ret = 0;
	int res = 0;
	
	struct nvlist_list rep = nvlist_create(2);
	
	nvlist_addpair(&rep, "k1", "v1");
	nvlist_addpair(&rep, "k2", "v2");
	nvlist_addstr(&rep, "k3&v3", '&');
	nvlist_addstr(&rep, "k4&v+4", '&');
	nvlist_addstr(&rep, "k5&v+5", '&');
	
	/*
	int i;
	for (i=0; i<rep.length; i++) {
		printf("%s=%s\n", rep.items[i].key, rep.items[i].value);
	}*/
	
	ret = strcmp(rep.items[0].key, "k1"); res += ret;
	printf("r: %d key  , %s, %s\n", ret, rep.items[0].key, "k1");
	ret = strcmp(rep.items[0].value, "v1"); res += ret;
	printf("r: %d value, %s, %s\n", ret, rep.items[0].value, "v1");
	
	ret = strcmp(rep.items[1].key, "k2"); res += ret;
	printf("r: %d key  , %s, %s\n", ret, rep.items[1].key, "k2");
	ret = strcmp(rep.items[1].value, "v2"); res += ret;
	printf("r: %d value, %s, %s\n", ret, rep.items[1].value, "v2");
	
	ret = strcmp(rep.items[2].key, "k3"); res += ret;
	printf("r: %d key  , %s, %s\n", ret, rep.items[2].key, "k3");
	ret = strcmp(rep.items[2].value, "v3"); res += ret;
	printf("r: %d value, %s, %s\n", ret, rep.items[2].value, "v3");
	
	ret = strcmp(rep.items[3].key, "k4"); res += ret;
	printf("r: %d key  , %s, %s\n", ret, rep.items[3].key, "k4");
	ret = strcmp(rep.items[3].value, "v+4"); res += ret;
	printf("r: %d value, %s, %s\n", ret, rep.items[3].value, "v+4");
	
	//free(rep.items);
	
	return res;
}
#endif
