#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../stringlib.h"

struct nvlist_pair {
	char *key;
	char* value;
};

struct nvlist_list {
	struct nvlist_pair *items;
	int length;
	int max;
};

int nvlist_addpair(struct nvlist_list *rep, char* key, char* value) {
	struct nvlist_pair p;
	p.key = key;
	p.value = value;
	rep->items[rep->length++] = p;
	return rep->length;
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


#ifdef NVLIST_MAIN
int main(int argc, char** argv, char **envp) {
	
	struct nvlist_list rep = nvlist_create(50);
	
	nvlist_addpair(&rep, "k1", "v1");
	nvlist_addpair(&rep, "k2", "v2");
	
	int i;
	for (i=0; i<rep.length; i++) {
		printf("%s=%s\n", rep.items[i].key, rep.items[i].value);
		free(rep.items[i].key);
	}
	
	free(rep.items);
	
	return 0;
}
#endif