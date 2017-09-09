#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../stringlib.h"

struct nvlist_pair {
	char *key;
	char* value;
};

struct nvlist_replacements {
	struct nvlist_pair items[100];
	int length;
};

int nvlist_addpair(struct nvlist_replacements *rep, char* key, char* value) {
	struct nvlist_pair p;
	p.key = key;
	p.value = value;
	rep->items[rep->length++] = p;
	return rep->length;
}


int main(int argc, char** argv, char **envp) {
	
	struct nvlist_replacements rep;
	rep.length = 0;
	
	nvlist_addpair(&rep, "k1", "v1");
	nvlist_addpair(&rep, "k2", "v2");
	
	int i;
	for (i=0; i<rep.length; i++)
		printf("%s=%s\n", rep.items[i].key, rep.items[i].value);
	
	return 0;
}