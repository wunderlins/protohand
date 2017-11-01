#include "cmd_parser.h"

int find_var_value(char* varname, struct nvlist_list* query, char** result) {
    int ii;
    
	//printf("---> %s\n", varname);
	
    // environment variables
    if (varname[0] == 'e' && varname[1] == 'n' &&
        varname[2] == 'v' && varname[3] == '.' ) {
        char* envarname = (char *) malloc(sizeof(char*) * (strlen(varname)+1));
        strcpy(envarname, varname+4);
        //printf("envarname: '%s'\n", envarname);
        char* value;
        //printf("getenv: '%s'\n", getenv(envarname));
        value = getenv(envarname);
        //printf("value   : %s\n", value);
        if (value == NULL)
            return 1;
        
        //printf("--> %s value:   %s\n", varname, value);
        *result = value;
        return 0;
    }
    
    // query variables
    //printf("l: %d\n", query->length);
    for (ii=0; ii<query->length; ii++) {
        //printf("--> key: %s\n", query->items[ii].key);
        if(strcmp(query->items[ii].key, varname) == 0) {
            *result = query->items[ii].value;
            return 0;
        }
    }
    
	return 1;
}

/**
 * parse the conditional
 * schema: var[ \t]*[!]=[ \t]*var[ \t]*:[ \t]*value[ \t]*
 */
int parse_conditional(char* varname, t_conditional* cond, struct nvlist_list* query) {
	
	int ii = 0;
	int pos = -1;
	int ll = strlen(varname);
	int ret = 0;
	
	// reset
	cond->var1 = (char*) "";
	cond->var2 = (char*) "";
	cond->sign = 0;
	cond->replace = (char*) "";
	cond->match = -1;
	
	// var name 1
	for(ii=0; ii<ll; ii++) {
		if (varname[ii] == '=') {
			if (varname[ii-1] == '!')
				cond->sign = -1;
			else
				cond->sign = 1;
			if (varname[ii+1] == '=')
				ii++; // skip second equal
			pos = ii;
			break;
		}
	}
	
	//printf("varname %s, ii %d, pos %d\n", varname, ii, pos);
	
	if(pos == -1)
		return EXP_ERR_NO_EQUAL;
	
	cond->var1 = (char*) malloc(sizeof(char*) * (pos + 1));
	int v1l = pos;
	if (cond->sign < 0)
		v1l--;
	strncpy(cond->var1, varname, v1l);
	cond->var1[v1l] = 0;
	trim(cond->var1);
	ii=0;
	while(cond->var1[ii] == ' ') cond->var1++;
	//printf("var1: '%s'\n", cond->var1);
	
	//return 0;
	
	// var name 2
	int var2start = pos+1;
	int var2end = -1;
	for(ii=var2start; ii<ll; ii++) {
		if (varname[ii] == ':') {
			var2end = ii;
			break;
		}
	}
	
	if (var2end == -1)
		return EXP_ERR_NO_COLON;
	
	//printf("va2s: %d, var2e: %d\n", var2start, var2end);
	cond->var2 = (char*) malloc(sizeof(char*) * (var2end - var2start + 1));
	strncpy(cond->var2, varname+var2start, var2end - var2start);
	trim(cond->var2);
	ii=0;
	while(cond->var2[ii] == ' ') cond->var2++;
	
	//printf("var2: '%s'\n", cond->var2);
	
	// result
	cond->replace = (char*) malloc(sizeof(char*) * (ll + var2end + 1));
	strcpy(cond->replace, varname+var2end+1);
	trim(cond->replace);
	ii=0;
	while(cond->replace[ii] == ' ') cond->replace++;
	//printf("replace: '%s'\n", cond->replace);	
	
	char* value1;
	ret = find_var_value(cond->var1, query, &value1);
	if (ret == 1)
		return EXP_ERR_QUERYNVVAR_NOT_FOUND;

	char* value2;
	ret = find_var_value(cond->var2, query, &value2);
	if (ret == 1)
		return EXP_ERR_QUERYNVVAR_NOT_FOUND;

	// compare values
	if (cond->sign == 1 && strcmp_lcase(value1, value2) == 0)
		cond->match = 1;
	
	if (cond->sign == -1 && strcmp_lcase(value1, value2) != 0)
		cond->match = 1;
	
	return 0;
}

int append_resize(char* string, char* append, int bufflength, int blocksize) {
	int new_l = strlen(string) + strlen(append) + 1;
	if (new_l > bufflength) { // realloc mor memmory
		int diff = bufflength - new_l;
		int add  = (diff/BLOCKSIZE*BLOCKSIZE)+BLOCKSIZE;
		string = (char *) realloc(string, bufflength+add);
		if (string == NULL)
			return 0;
		strcat(string, append);
		return bufflength+add;
	}
	
	strcat(string, append);
	return bufflength;
}

char* expandvar_err_var_name = "";

int expand_vars(char** str, struct nvlist_list* query) {
	int i, open, start;
	int l = strlen(str[0]);
	open = 0; i = 0; start = 0;
	int has_colon = 0;
	int has_equal = 0;
	int buffer_length = 0;
	int ret;
	char* result;
	t_conditional cond = {(char*) "", (char*) "", 0, (char*) "", -1};
	
	char* out = (char*) malloc(sizeof(char*) * BLOCKSIZE);
	if (out == NULL) {
		//printf("Failed malloc 1\n");
		return EXP_ERR_MALLOC;
	}
	buffer_length = BLOCKSIZE;
	out[0] = 0;
	
	for (i=0; i<l; i++) {
		//printf("%d %d %c\n", has_colon, has_equal, str[0][i]);
		if (i > 0 && str[0][i] == '{' && str[0][i-1] == '$') {
			//printf("Begin: %d\n", i);
			open = 1;
			start = i;
			out[strlen(out)-1] = 0; // remove '$' from out string
			has_colon = 0;
			has_equal = 0;
			continue;
		}
		
		if (str[0][i] == '}') {
			//printf("End:   %d\n", i);
			open = 0;
			
			int len = (i-start-1);
			
			//printf("len: %d\n", len);
			char* varname = (char *) malloc(sizeof(char*) * (len+1));
			if (varname == NULL) {
				//printf("Failed malloc 2\n");
				return EXP_ERR_MALLOC;
			}
			
			strncpy(varname, *(str)+start+1, len);
			varname[len] = 0;
			
			//printf("varname: %s\n", varname);
			
			// resolve expression
			if (has_colon == 1 && has_equal == 1) {
				//printf("resolving expression %s\n", varname);
				ret = parse_conditional(varname, &cond, query);
				
				//printf("var1: '%s', var2 '%s', replace: '%s'\n",
				//       cond.var1, cond.var2, cond.replace);
				
				if (cond.match == 1) {
					ret = append_resize(out, cond.replace, buffer_length, BLOCKSIZE);
					if (ret == 0) {
						//printf("Failed remalloc \n");
						return EXP_ERR_REALLOC;
					}
					
					buffer_length = ret;
				}

			// resolve query and env variables
			} else { // variable
				//printf("Resolving variable\n");
				ret = find_var_value(varname, query, &result);
				//printf("result: %d, result: %s\n", ret, result);
				if (ret == 1) {
					//printf("Var value Result: %s, ret: %d\n", result, ret);
					expandvar_err_var_name = varname;
					return EXP_ERR_QUERYNVVAR_NOT_FOUND;
				}
				
				ret = append_resize(out, result, buffer_length, BLOCKSIZE);
				//printf("--> out: %s\n", out);
				if (ret == 0) {
					//printf("Failed remalloc \n");
					return EXP_ERR_REALLOC;
				}
				
				buffer_length = ret;
			}
			
			has_colon = 0;
			has_equal = 0;
			continue;
		}
		
		if (str[0][i] == ':') has_colon = 1;
		if (str[0][i] == '=') has_equal = 1;
		
		if (open == 0) {
			char append[2];
			append[0] = str[0][i];
			append[1] = 0;
			ret = append_resize(out, append, buffer_length, BLOCKSIZE);
			if (ret == 0) {
				//printf("Failed remalloc \n");
				return EXP_ERR_REALLOC;
			}
			buffer_length = ret;
		}
	}
	
	//printf("out: %s\n", out);
	*str = out;
	
	return 0;
}

#ifdef CMD_PARSER_MAIN

int asserts(char* str1, char* str2) {
	if (strcmp(str1, str2) == 0) 
		return 0;
	return 1;
}

int cmdparser_test(char* uri, char** cmd, char* expect) {
	int ret = 0;
	int res = 0;
	
	struct t_uri uri_parsed = uriparse_create(uri);
	ret = uriparse_parse(uri, &uri_parsed);
	if (ret != 0) {
		printf("Parser Error %d, %s\n", ret, uri);
		return ret;
	}
	
	ret = expand_vars(cmd, &uri_parsed.nvquery);
	res = asserts(*cmd, expect);
	printf("res %d, ret: %d, out: %s, exp: %s\n", res, ret, *cmd, expect);
	
	return res;
}

int main(int argc, char*argv[]) {
	int ret = 0;
	int res = 0;
	
	char* uri;
	char* cmd; 
	char* expect;
	
	putenv("windir=C:\\WINDOWS");
	putenv("USERNAME=wus");
	
	// match with spaces in expression
	uri = (char*) "proto:auth/path?name1=vaue1&name2=xxx";
	cmd = (char*) "${env.windir}\\notepad.exe /A \"${name1}\" ${name2} ${ env.USERNAME != name2 :--debug}";
	expect = (char*) "C:\\WINDOWS\\notepad.exe /A \"vaue1\" xxx --debug";
	
	// match without spaces in expression
	ret = cmdparser_test(uri, &cmd, expect);
	//printf("ret: %d\n", ret);
	
	cmd = (char*) "${env.windir}\\notepad.exe /A \"${name1}\" ${name2} ${env.USERNAME!=name2:--debug}";
	ret = cmdparser_test(uri, &cmd, expect);
	res += ret;
	//printf("ret: %d\n", ret);
	
	// no match
	cmd = (char*) "${env.windir}\\notepad.exe /A \"${name1}\" ${name2} ${env.USERNAME=name2:--debug}";
	expect = (char*) "C:\\WINDOWS\\notepad.exe /A \"vaue1\" xxx ";
	ret = cmdparser_test(uri, &cmd, expect);
	res += ret;
	//printf("ret: %d\n", ret);
	
	// case insensitive comparison
	uri = (char*) "proto:auth/path?name1=vaue1&name2=xxx";
	cmd = (char*) "${env.windir}\\notepad.exe /A \"${name1}\" ${name2} ${env.MYVAR=name2:--debug}";
	putenv("MYVAR=XXX");
	expect = (char*) "C:\\WINDOWS\\notepad.exe /A \"vaue1\" xxx --debug";
	ret = cmdparser_test(uri, &cmd, expect);
	res += ret;
	//printf("ret: %d\n", ret);
	
	return res;
}

#endif // CMD_PARSER_MAIN

// proto:auth/path?name1=vaue1&name2=val
