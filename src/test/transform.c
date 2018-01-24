/**
 * Transformation for URL parameters
 * 
 * It might be necessary tro transform urlparameters. For example 
 * leading zeros mmust be added or removed.
 * 
 * Sometimes it is required to use the same transformation function 
 * several times with the same parameter (for example printf(%08d). 
 * For convenience such functiosn can be regisetered as alias.
 * 
 * Aliases are stored in `alias_lsit_t` which holds the alias name 
 * and the real function including it's default parmameters.
 * 
 * `fn_list_t` contains the function definition.
 * 
 * `transform_list_t` contains a list of all parameters and their 
 * respective transformation function defined in `transform_item_t`.
 * 
 * 2017, Simon Wunderlin
 */ 

#include "transform.h"
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_TRANSFORM_ITEM {"", "", "", "", NULL, NULL, 0}
typedef struct transform_item {
	char* name;
	char* value_in; 
	char* value_out; 
	char* method;
	void* fn; // pointer to fn_t
	char** args;
	int transformed; 
} transform_item_t;

typedef struct transform_list {
	transform_item_t *items;
	int length;
} transform_list_t;

typedef struct fn {
	char* name;
	int   numargs;
	void (*func)(transform_item_t* item);
} fn_t;

typedef struct fn_list {
	fn_t *items;
	int length;
} fn_list_t;

int get_function_def(struct nvlist_list *func) {
	//struct nvlist_list func = nvlist_create(0);
	int ret;
	ret = nvlist_addpair(func, "fallnr_zero", "printf(%08d)");
	if (ret < 0) return ret;
	ret = nvlist_addpair(func, "patnr_zero", "printf(%08d)");
	if (ret < 0) return ret;
	ret = nvlist_addpair(func, "fallnr_nozero", "ltrimzero");
	if (ret < 0) return ret;
	ret = nvlist_addpair(func, "patnr_nozero", "ltrimzero");
	if (ret < 0) return ret;
	
	return 0;
}

int add_func(fn_list_t* funcs, fn_t* fnc) {
	if (funcs->length == 0) {
		funcs->items = malloc(sizeof(fn_t) * 1);
		if (funcs->items == NULL)
			return -1;
		funcs->length = 1;
	} else {
		funcs->items = realloc(funcs->items, sizeof(fn_t) * (funcs->length+1));
		if (funcs->items == NULL)
			return -1;
		funcs->length++;
	}
	
	funcs->items[funcs->length - 1] = *fnc;
	
	return 0;
}

void my_printf(transform_item_t* item) {
	
}

void ltrimzero(transform_item_t* item) {
	//printf("in: %s\n", item->value_in);
	
	char *tmp = (char*) malloc(sizeof(char) * (strlen(item->value_in)+1));
	strcpy(tmp, item->value_in);
	
	while((tmp[0] == '0' || tmp[0] == ' ') && tmp[0] != 0)
		tmp++;

	//printf("tmp: %s\n", tmp);
	item->value_out = (char*) malloc(sizeof(char) * (strlen(tmp)+1));
	strcpy(item->value_out, tmp);
	
	item->transformed = 1;
}

int run(transform_item_t* item) {
	//printf("in:  %s\n", item->value_in);
	
	if (item->fn == NULL) {
		fprintf(stdout, "Error: Unknown function '%s' for parameter "
		                "'%s'\n", item->method, item->name);
		return 1;
	}

	// dereference void pointer and set type, then run the method
	fn_t *cfn = (fn_t*) item->fn;
	
	cfn->func(item);
	//printf("out: %s\n", item->value_out);
	
	return 0;
}

#ifdef TRANSFORM_MAIN
int main(int argc, char** argv) {
	
	int i;
	struct nvlist_list registry = nvlist_create(0);
	get_function_def(&registry);
	
	fn_t fn_ltrimzero = {"ltrimzero", -1, NULL};
	fn_ltrimzero.numargs = 0;
	fn_ltrimzero.func = &ltrimzero;	
	
	fn_list_t funcs;
	funcs.length = 0;
	add_func(&funcs, &fn_ltrimzero);
	 
	transform_item_t item = DEFAULT_TRANSFORM_ITEM;
	item.name = "FALLNR";
	item.value_in = "0000005";
	item.method = "ltrimzero";
	for (i=0; i<funcs.length; i++) {
		if (strcmp(item.method, funcs.items[i].name) == 0) {
			item.fn = &funcs.items[0]; // fn_ltrimzero
			break;
		}
	}
	
	printf("in:  %s\n", item.value_in);
	run(&item);
	printf("out: %s\n", item.value_out);
	return 0;
	
	struct nvlist_list rep = nvlist_create(2);
	
	nvlist_addpair(&rep, "k1", "v1");
	nvlist_addpair(&rep, "k2", "v2");
	
	nvlist_addstr(&rep, "k3&v3", '&');
	nvlist_addstr(&rep, "k4&v+4", '&');
	nvlist_addstr(&rep, "k5&v+5", '&');
	
	transform_list_t tlist;
	tlist.items = malloc(sizeof(transform_item_t) * rep.length);
	tlist.length = rep.length;
	
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
