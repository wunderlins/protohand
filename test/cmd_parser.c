#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../lib/stringlib.h"
#include "../lib/nvlist.h"
#include "../lib/uriparse.h"

int find_var_value(char* name, struct nvlist_list* query, char** result) {
	
	return 0;
}

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
				return 2;
			
			strncpy(varname, *(str)+start+1, len);
			varname[len] = 0;
			
			//printf("varname: %s\n", varname);
			
			// type
			if (has_colon && has_equal) {
				printf("conditional: %s\n", varname);
				
				// parse the conditional
				// schema: var[ \t]*[!]=[ \t]*var[ \t]*:[ \t]*value[ \t]*
				char* var1;
				char* var2;
				char* replace;
				int sign = 0;
				int pos = -1;
				int ll = strlen(varname);
				
				// var name 1
				for(ii=0; ii<ll; ii++) {
					if (varname[ii] == '=') {
						if (varname[ii-1] == '!')
							sign = -1;
						else
							sign = 1;
						pos = ii;
						break;
					}
				}
				
				if(pos == -1)
					return 5;
				
				var1 = (char*) malloc(sizeof(char*) * (pos + 1));
				int v1l = pos;
				if (sign < 0)
					v1l--;
				strncpy(var1, varname, v1l);
				var1[v1l] = 0;
				trim(var1);
				ii=0;
				while(var1[ii] == ' ') var1++;
				printf("var1: '%s'\n", var1);
				
				// var name 2
				int var2start = pos+1;
				int var2end = -1;
				for(ii=var2start; ii<ll; ii++) {
					if (varname[ii] == ':') {
						var2end = ii;
						break;
					}
				}
				
				printf("va2s: %d, var2e: %d\n", var2start, var2end);
				var2 = (char*) malloc(sizeof(char*) * (var2end - var2start + 1));
				strncpy(var2, varname+var2start, var2end - var2start);
				trim(var2);
				ii=0;
				while(var2[ii] == ' ') var2++;
				
				printf("var2: '%s'\n", var2);
				
				// result
				replace = (char*) malloc(sizeof(char*) * (ll + var2end + 1));
				strcpy(replace, varname+var2end+1);
				trim(replace);
				ii=0;
				while(replace[ii] == ' ') replace++;
				printf("replace: '%s'\n", replace);
				
				int lll = strlen(var1);
				char* value1;
				char* value2;
				if (var1[0] == 'e' && var1[1] == 'n' && 
					var1[2] == 'v' && var1[3] == '.' ) {
					char* envarname = (char *) malloc(sizeof(char*) * (lll+1));
					strcpy(envarname, var1+4);
					char* value1 = getenv(envarname);
					if (value1 == NULL)
						return 1;
					printf("value1: '%s'\n", value1);
				} else {
					;
				}
				
				
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
	char* cmd = (char*) "${env.WINDIR}\\notepad.exe /A \"${name1}\" ${ env.USERNAME != name2 : --debug }";
	
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