#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../lib/stringlib.h"
#include "../lib/nvlist.h"
#include "../lib/uriparse.h"

int expand_vars(char** str) {
	int i, o, open, start;
	int l = strlen(str[0]);
	open = 0; i = 0; o = 0; start = 0;
	
	char* out = (char*) malloc(sizeof(char*) * 1);
	if (out == NULL)
		return 2;
	out[0] = 0;
	
	for (i=0; i<l; i++) {
		if (i > 0 && str[0][i] == '{' && str[0][i-1] == '$') {
			//printf("Begin: %d\n", i);
			open = 1;
			o--;
			start = i;
			continue;
		}
		
		if (str[0][i] == '}') {
			//printf("End:   %d\n", i);
			open = 0;
			
			int len = (i-start-1);
			
			char* varname = (char *) malloc(sizeof(char*) * (len+1));
			if (varname == NULL)
				return 2;
			strncpy(varname, *(str)+start+1, len);
			varname[len] = 0;
			char* value = getenv(varname);
			
			if (value == NULL)
				return 1;
			
			//printf("Var(%d): %s=%s\n", len, varname, value);
			
			out = (char *) realloc(out, o+strlen(value)+1);
			out[o] = 0;
			strcat(out, value);
			o+= strlen(value);
			
			continue;
		}
		
		if (open == 0) {
			out = (char *) realloc(out, o+1);
			if (out == NULL)
				return 2;
			out[o++] = str[0][i];
			out[o] = 0;
		}
	}
	
	//printf("out: %s\n", out);
	*str = out;
	
	return 0;
}

#ifdef CMD_PARSER_MAIN
int main(int argc, char*argv[]) {
	
	int ret = 0;
	int i = 0;
	int res;
	
	char* uri = (char*) "proto:auth/path?name1=value1&name2=value+2";
	
	struct t_uri uri_parsed = uriparse_create(uri);
	res = uriparse_parse(uri, &uri_parsed);
	if (res != 0) {
		ret = 127+res;
		printf("Parser Error %d, %s\n", res, uri);
		return ret;
	}
	
	nvlist_pair *p;
	for (i=0; i<uri_parsed.nvquery.length; i++) {
		p = &uri_parsed.nvquery.items[i];
		printf("k: %s, v: %s\n", p->key, p->value);
	}
	
	return 0;
}

#endif // CMD_PARSER_MAIN