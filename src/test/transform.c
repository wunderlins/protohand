#include "transform.h"
#include <stdarg.h>
#include <stdlib.h>

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

int get_function_def(struct nvlist_list *func) {
	//struct nvlist_list func = nvlist_create(0);
	int ret;
	ret = nvlist_addpair(func, "fallnr_zero", "printf(%08d)");
	if (ret < 0) return ret;
	ret = nvlist_addpair(func, "patnr_zero", "printf(%08d)");
	if (ret < 0) return ret;
	ret = nvlist_addpair(func, "fallnr_nozero", "str2int");
	if (ret < 0) return ret;
	ret = nvlist_addpair(func, "patnr_nozero", "str2int");
	if (ret < 0) return ret;
	
	return 0;
}

typedef struct {
	char* name;
	int   numargs;
	char** arguments;
	void (*func)(char* value, ...);
} fn_t;


void my_printf(char *format, ...) {
	va_list args;
	va_start(args, format);
	printf(format, args);
	va_end(args);
}

#ifdef TRANSFORM_MAIN
int main(int argc, char** argv) {

	struct nvlist_list func = nvlist_create(0);
	get_function_def(&func);
	//printf("func length %d\n", func.length);
	
	//typedef int (*fn_printf_ptr) (char const *str, ...);
	fn_t fn_printf = {"printf", -1, NULL, NULL};
	//fn_printf.func = &printf;
	
	//void (*ptr_printf)(char *str, ...);
	fn_printf.func = &my_printf;
	
	fn_printf.func("blah, %d\n", 1);
	

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
