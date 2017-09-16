#include "uriparse.h"

// placeholder for an empty string
char* empty = "";

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

int nvlist_resize(struct nvlist_list* rep, int size) {
	if (size <= rep->max)
		return -2;
	
	struct nvlist_pair* tmp = realloc(rep->items, sizeof(struct nvlist_pair*) * size);
	if (tmp)
		rep->items = tmp;
	else
		return -1;
	
	rep->max = size;
	return rep->max;
}

struct nvlist_list nvlist_create(int size) {
	struct nvlist_list rep;
	rep.items = malloc(sizeof(struct nvlist_pair*) * size);
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

int nvlist_find_parts(char* string, char search) {
	int i, found = 0;
	int len = strlen(string);
	for(i=0; i<len; i++)
		if (string[i] == search)
			found++;
	
	return found;
}

struct t_uri uriparse_create(char* uri) {
	struct t_uri uri_parsed = {uri, empty, empty, empty, empty, empty, {-1, -1, -1, -1, -1, -1, -1}};
	uri_parsed.nvquery = nvlist_create(0);
	return uri_parsed;
}

int parse(char* uri, struct t_uri* uri_parsed) {
	int doubleslash = 0;
	int stage = 1;
	int i = 0;
	int length = strlen(uri);
	
	//struct t_uri uri_parsed = {uri, empty, empty, empty, empty, empty, {-1, -1, -1, -1, -1, -1, -1}};
	uri_parsed->pos[0] = 0;
	uri_parsed->pos[FOUND_END] = length;
	
	for (i=0; i<length; i++) {
		
		#if DEBUG > 3
		printf("[%04d] %c %d\n", i, uri[i], stage);
		#endif
		if (stage < FOUND_AUTHORITY && uri[i] == ':') {
			stage = FOUND_AUTHORITY;
			uri_parsed->pos[FOUND_PROTO] = i;
			#if DEBUG > 3
			printf(", found proto\n");
			#endif
			if (uri[i+1] == '/' && uri[i+2] == '/') {
				#if DEBUG > 3
				printf(", have double slash\n");
				#endif
				i=i+3;
				doubleslash = 2;
			}
			continue;
		}
		      
		if (stage < FOUND_PATH && uri[i] == '/') {
			stage = FOUND_PATH;
			uri_parsed->pos[FOUND_PATH] = i;
			if (length == i+1)
				uri_parsed->pos[FOUND_PATH]++;
			#if DEBUG > 3
			printf(", found path\n");
			#endif
			continue;
		}

		if (stage < FOUND_QUERY && uri[i] == '?') {
			stage = FOUND_QUERY;
			uri_parsed->pos[FOUND_QUERY] = i;
			if (length == i+1)
				uri_parsed->pos[FOUND_QUERY]++;
			#if DEBUG > 3
			printf(", found query\n");
			#endif
			continue;
		}

		if (stage < FOUND_FRAGMENT && uri[i] == '#') {
			stage = FOUND_FRAGMENT;
			uri_parsed->pos[FOUND_FRAGMENT] = i;
			if (length == i+1)
				uri_parsed->pos[FOUND_FRAGMENT]++;
			#if DEBUG > 3
			printf(", found fragment\n");
			#endif
			continue;
		}

	}
	
	// check if we have an authority
	if (uri_parsed->pos[FOUND_AUTHORITY] == -1) {
		if (uri_parsed->pos[FOUND_PATH] != -1)
			uri_parsed->pos[FOUND_AUTHORITY] = uri_parsed->pos[FOUND_PATH];
	}
	
	if (uri_parsed->pos[FOUND_AUTHORITY] == -1) {
		if (uri_parsed->pos[FOUND_QUERY] != -1)
			uri_parsed->pos[FOUND_AUTHORITY] = uri_parsed->pos[FOUND_QUERY];
	}
	
	if (uri_parsed->pos[FOUND_AUTHORITY] == -1) {
		if (uri_parsed->pos[FOUND_FRAGMENT] != -1)
			uri_parsed->pos[FOUND_AUTHORITY] = uri_parsed->pos[FOUND_FRAGMENT];
	}

	if (uri_parsed->pos[FOUND_AUTHORITY] == -1&& 
	    uri_parsed->pos[FOUND_PROTO]+1 < length) {
		uri_parsed->pos[FOUND_AUTHORITY] = length;
	}
	
	// check for required parameters
	if (uri_parsed->pos[FOUND_PROTO] == -1 ) 
		return FOUND_PROTO;
	
	if (uri_parsed->pos[FOUND_AUTHORITY] == -1 ) 
		return FOUND_AUTHORITY;
	
	// start chopping strings out of the original string
	uri_parsed->proto = malloc(sizeof(char*) * uri_parsed->pos[FOUND_PROTO]+1);
	strncpy(uri_parsed->proto, uri_parsed->uri, uri_parsed->pos[FOUND_PROTO]);
	uri_parsed->proto[uri_parsed->pos[FOUND_PROTO]] = '\0';
	
	uri_parsed->authority = malloc(sizeof(char*) * (uri_parsed->pos[FOUND_AUTHORITY]+1));
	strncpy(uri_parsed->authority, 
	        uri_parsed->uri+(uri_parsed->pos[FOUND_PROTO]+doubleslash+1), 
	        uri_parsed->pos[FOUND_AUTHORITY]-uri_parsed->pos[FOUND_PROTO]-doubleslash-1);
	uri_parsed->authority[uri_parsed->pos[FOUND_AUTHORITY]-uri_parsed->pos[FOUND_PROTO]-doubleslash-1] = '\0';
	
	#if DEBUG > 1
	printf("proto:     %s\n", uri_parsed->proto);
	printf("authority: %s\n", uri_parsed->authority);
	#endif
	
	int len;
	if (uri_parsed->pos[FOUND_PATH] != -1) {
		
		if (uri_parsed->pos[FOUND_QUERY] != -1)
			len = uri_parsed->pos[FOUND_QUERY] - uri_parsed->pos[FOUND_PATH];
		else if (uri_parsed->pos[FOUND_FRAGMENT] != -1)
			len = uri_parsed->pos[FOUND_FRAGMENT] - uri_parsed->pos[FOUND_PATH];
		else
			len = length - uri_parsed->pos[FOUND_PATH];
		
		uri_parsed->path = malloc(sizeof(char*) * (length+1));
		strncpy(uri_parsed->path, 
				uri_parsed->uri+uri_parsed->pos[FOUND_PATH]+1, // skip '/'
				len);
		uri_parsed->path[len-1] = '\0';
	}
	#if DEBUG > 1
	printf("path:      %s\n", uri_parsed->path);
	#endif
	
	if (uri_parsed->pos[FOUND_QUERY] != -1) {
		
		if (uri_parsed->pos[FOUND_FRAGMENT] != -1)
			len = uri_parsed->pos[FOUND_FRAGMENT] - uri_parsed->pos[FOUND_QUERY];
		else
			len = length - uri_parsed->pos[FOUND_QUERY];
		
		uri_parsed->query = malloc(sizeof(char*) * (length+1));
		strncpy(uri_parsed->query, 
				uri_parsed->uri+uri_parsed->pos[FOUND_QUERY]+1, // skip '?'
				len);
		uri_parsed->query[len-1] = '\0';
	}
	#if DEBUG > 1
	printf("query:     %s\n", uri_parsed->query);
	#endif
	
	if (uri_parsed->pos[FOUND_FRAGMENT] != -1) {
		
		len = length - uri_parsed->pos[FOUND_FRAGMENT];
		
		uri_parsed->fragment = malloc(sizeof(char*) * (length+1));
		strncpy(uri_parsed->fragment, 
				uri_parsed->uri+uri_parsed->pos[FOUND_FRAGMENT]+1, // skip '#'
				len);
		uri_parsed->fragment[len-1] = '\0';
	}
	#if DEBUG > 1
	printf("fragment:  %s\n", uri_parsed->fragment);
	#endif
	
	// parse query string
	int qparts = nvlist_find_parts(uri_parsed->query, '&');
	
	// we have a query string, but it is not '&' delimited.
	// add one element, with the name as the query, urlescape it
	if (qparts == 0 && uri_parsed->query != empty) {
		int r = nvlist_resize(&(uri_parsed->nvquery), 1);
		printf("nvlist length [%d], max [%d], key: '%s'\n", uri_parsed->nvquery.length, r, 
		                                                    uri_parsed->nvquery.items[0].key);
		
		char* escaped = malloc(sizeof(char*) * (strlen(uri_parsed->query)+1));
		strcpy(escaped, uri_parsed->query);
		// FIXME: we somehow overwrite uri_parsed->proto instead of 
		//        uri_parsed->nvquery.items[0].key
		char *k = ((uri_parsed->nvquery).items[0]).key;
		printf("query: '%s'\n", k);
		urldecode2(k, escaped);
		printf("nvquery[0].key: '%s'\n", uri_parsed->nvquery.items[0].key);
		
	}
	
	// there are query parts, parse them into a nvlist
	if (qparts > 0) {
		;
	}
	
	// debug output
	#if DEBUG > 1
	printf("Start      [%d]\n", uri_parsed->pos[FOUND_START]);
	printf("Proto      [%d]\n", uri_parsed->pos[FOUND_PROTO]);
	printf("Authority  [%d]\n", uri_parsed->pos[FOUND_AUTHORITY]);
	printf("Path       [%d]\n", uri_parsed->pos[FOUND_PATH]);
	printf("Query      [%d]\n", uri_parsed->pos[FOUND_QUERY]);
	printf("Fragment   [%d]\n", uri_parsed->pos[FOUND_FRAGMENT]);
	printf("End        [%d]\n", uri_parsed->pos[FOUND_END]);
	#endif

	return 0;	
}

#ifdef URIPARSE_DEBUG
#if URIPARSE_DEBUG > 0
int uriparse_test(char* uri, char* proto, char* authority, char* path, char* query, char* fragment) {
	int ret = 0;
	int res;
	//struct t_uri uri_parsed = {uri, empty, empty, empty, empty, empty, {-1, -1, -1, -1, -1, -1, -1}};
	struct t_uri uri_parsed = uriparse_create(uri);
	res = parse(uri, &uri_parsed);
	if (res != 0) {
		ret = 127+res;
		printf("Parser Error %d, %s\n", res, uri);
		return ret;
	}

	if (strcmp(uri_parsed.proto, proto) != 0)
		ret = FOUND_PROTO;
	
	if (strcmp(uri_parsed.authority, authority) != 0)
		ret = FOUND_AUTHORITY;
	
	if (strcmp(uri_parsed.path, path) != 0)
		ret = FOUND_PATH;
	
	if (strcmp(uri_parsed.query, query) != 0)
		ret = FOUND_PATH;
	
	if (strcmp(uri_parsed.query, query) != 0)
		ret = FOUND_QUERY;
	
	if (strcmp(uri_parsed.fragment, fragment) != 0)
		ret = FOUND_FRAGMENT;

	if (ret == 0) {
		printf("OK     %s\n", uri);
	} else {
		printf("Failed %s\n", uri);
		printf("%-10s, %03d, check: '%s', result: '%s'\n", "proto", strcmp(uri_parsed.proto, proto), proto, uri_parsed.proto);
		printf("%-10s, %03d, check: '%s', result: '%s'\n", "authority", strcmp(uri_parsed.authority, authority), authority, uri_parsed.authority);
		printf("%-10s, %03d, check: '%s', result: '%s'\n", "path", strcmp(uri_parsed.path, path), path, uri_parsed.path);
		printf("%-10s, %03d, check: '%s', result: '%s'\n", "query", strcmp(uri_parsed.query, query), query, uri_parsed.query);
		printf("%-10s, %03d, check: '%s', result: '%s'\n", "fragment", strcmp(uri_parsed.fragment, fragment), fragment, uri_parsed.fragment);
	}
	
	return ret;
}

int main(int argc, char *argv[]) {
	
	// unit testing of the uri parser
	/*
	uriparse_test("proto:authority", "proto", "authority", empty, empty, empty);
	uriparse_test("proto:authority/p", "proto", "authority", "p", empty, empty);
	uriparse_test("proto:authority?q", "proto", "authority", empty, "q", empty);
	uriparse_test("proto:authority#f", "proto", "authority", empty, empty, "f");
	uriparse_test("proto:authority?q#f", "proto", "authority", empty, "q", "f");
	uriparse_test("proto:authority/p#f", "proto", "authority", "p", empty, "f");
	*/
	uriparse_test("proto:authority/p?q+1", "proto", "authority", "p", "q+1", empty);
	//uriparse_test("proto://authority/p?a=1&b=2&c=3#f", "proto", "authority", "p", "a=1&b=2&c=3", "f");
	
}
#endif
#endif