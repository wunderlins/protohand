#include "transform.h"

#define DEFAULT_TRANSFORM_ITEM {"", "", NULL, "", 0}
typedef struct {
	char* name;
	char* value_in; 
	char* value_out; 
	char* method;
	int transformed; 
} transform_item_t;

typedef struct {
	transform_item_t *items;
	int length;
} transform_list_t;

#ifdef TRANSFORM_MAIN
int main(int argc, char** argv) {
	struct nvlist_list rep = nvlist_create(2);
	
	nvlist_addpair(&rep, "k1", "v1");
	nvlist_addpair(&rep, "k2", "v2");
	nvlist_addstr(&rep, "k3&v3", '&');
	nvlist_addstr(&rep, "k4&v+4", '&');
	nvlist_addstr(&rep, "k5&v+5", '&');
	
	transform_list_t tlist;
	tlist.items = malloc(sizeof(transform_item_t) * rep.length);
	tlist.length = rep.length;
	
	int i;
	for (i=0; i<rep.length; i++) {
		transform_item_t item = DEFAULT_TRANSFORM_ITEM;
		tlist.items[i] = item;
		
		tlist.items[i].name = rep.items[i].key;
		tlist.items[i].value_in = rep.items[i].value;
		
		//printf("'%s': '%s'\n", rep.items[i].key, rep.items[i].value);
	}
	
	for (i=0; i<tlist.length; i++) {
		printf("'%s': '%s' (%s)\n", tlist.items[i].name, tlist.items[i].value_in, tlist.items[i].value_out);
	}
	
	return 0;
}
#endif
