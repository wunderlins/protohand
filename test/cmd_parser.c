#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../lib/stringlib.h"
#include "../lib/nvlist.h"
#include "../lib/uriparse.h"

int expand_vars(char** str, struct nvlist_list* query) {
	int i, o, open, start;
	int l = strlen(str[0]);
	open = 0; i = 0; o = 0; start = 0;
	int has_colon = 0;
	int has_equal = 0;
	int ii = 0;
	
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
			
			//printf("len: %d\n", len);
			char* varname = (char *) malloc(sizeof(char*) * (len+1));
			if (varname == NULL)
				return 5;
			
			strncpy(varname, *(str)+start+1, len);
			varname[len] = 0;
			
			//printf("varname: %s\n", varname);
			
			// type
			if (has_colon && has_equal) {
				printf("conditional: %s\n", varname);
				
				// parse the conditional
				// schema: var[ \t]*[!]=[ \t]*var[ \t]*:[ \t]*value[ \t]*
				
			} else if (varname[0] == 'e' && varname[1] == 'n' && 
			    varname[2] == 'v' && varname[3] == '.' ) {
				char* envarname = (char *) malloc(sizeof(char*) * (len+1));
				strcpy(envarname, varname+4);
				char* value = getenv(envarname);
				
				if (value == NULL)
					return 1;
				
				//printf("env name: '%s'='%s'\n", envarname, value);
				
				out = (char *) realloc(out, o+strlen(value)+1);
				out[o] = 0;
				strcat(out, value);
				o+= strlen(value);
			} else {
				//printf("regular var: %s, %d\n", varname, query->length);
				
				int found = 0;
				for (ii=0; ii<query->length; ii++) {
					//printf("--> key: %s\n", query->items[ii].key);
					if(strcmp(query->items[ii].key, varname) == 0) {
						
						out = (char *) realloc(out, o+strlen(query->items[ii].value)+1);
						out[o] = 0;
						strcat(out, query->items[ii].value);
						o+= strlen(query->items[ii].value);
						found = 1;
						break;
					}
				}
				
				if (!found) {
					return 3;
				}
			}
			
			has_colon = 0;
			has_equal = 0;
			continue;
		}
		
		if (str[0][i] == ':') has_colon = 1;
		if (str[0][i] == '=') has_equal = 1;
		
		if (open == 0) {
			out = (char *) realloc(out, o+1);
			if (out == NULL)
				return 4;
			out[o++] = str[0][i];
			out[o] = 0;
		}
	}
	
	printf("out: %s\n", out);
	*str = out;
	
	return 0;
}

#ifdef CMD_PARSER_MAIN
int main(int argc, char*argv[]) {
	
	int ret = 0;
	int i = 0;
	int res;
	
	char* uri = (char*) "proto:auth/path?name1=value1&name2=value+2";
	char* cmd = (char*) "${env.WINDIR}\\notepad.exe /A \"${name1}\" ${env.USERNAME!=name2:--debug}";
	
	struct t_uri uri_parsed = uriparse_create(uri);
	res = uriparse_parse(uri, &uri_parsed);
	if (res != 0) {
		ret = 127+res;
		printf("Parser Error %d, %s\n", res, uri);
		return ret;
	}
	
	/*
	nvlist_pair *p;
	for (i=0; i<uri_parsed.nvquery.length; i++) {
		p = &uri_parsed.nvquery.items[i];
		printf("k: %s, v: %s\n", p->key, p->value);
	}
	*/
	
	ret = expand_vars(&cmd, &uri_parsed.nvquery);
	
	return ret;
}

#endif // CMD_PARSER_MAIN