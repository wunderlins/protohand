#include "nvlist.h"

int nvlist_addpair(struct nvlist_list *rep, char* key, char* value) {
	struct nvlist_pair p;
	p.key = key;
	p.value = value;
	rep->items[rep->length++] = p;
	return rep->length;
}

int nvlist_addstr(struct nvlist_list *rep, char* str, char delim) {
	int i = 0;
	int len = strlen(str);
	for(i=0; i< len; i++) {
		if (str[i] == delim) {
			char *key = malloc(sizeof(char*) * (i+1));
			char *val = malloc(sizeof(char*) * len);
			strncpy(key, str, i);
			key[i] = '\0';
			strncpy(val, str+i+1, len);
			
			return nvlist_addpair(rep, key, val);
		}
	}
	
	return -1;
}

struct nvlist_list nvlist_create(int size) {
	struct nvlist_list rep;
	rep.items = malloc(sizeof(struct nvlist_pair *) * size);
	rep.length = 0;
	rep.max = size;
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
	//printf(" -> size: %d, max: %d\n", size, rep->max);
	if (size <= rep->max)
		return -2;
	
	struct nvlist_pair* tmp = realloc(rep->items, sizeof(struct nvlist_pair*) * size);
	//printf("realloc ret: %d\n", tmp);
	if (!tmp)
		return -1;
	
	//printf(" -> copy items (%d)\n", rep->length);
	int i;
	for(i=0; i < rep->length; i++) {
		//printf(" -> %d: \n", i);
		tmp[i] = rep->items[i];
	}
	
	rep->items = tmp;
	
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
	
	struct nvlist_list rep = nvlist_create(50);
	
	nvlist_addpair(&rep, "k1", "v1");
	nvlist_addpair(&rep, "k2", "v2");
	nvlist_addstr(&rep, "k3&v3", '&');
	
	// FIXME: umlauts don't work on W10 msys with a us codepage
	//printf("ö\n"); // works if file uses CP 'OEM 852' 
	
	int i;
	for (i=0; i<rep.length; i++) {
		printf("%s=%s\n", rep.items[i].key, rep.items[i].value);
		free(rep.items[i].key);
	}
	
	free(rep.items);
	
	return 0;
}
#endif