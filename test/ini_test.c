/* ini.h example that simply dumps an INI file without comments */

#include <stdio.h>
#include <string.h>
#include "ini.h"

typedef struct
{
	const char* section;
	const char* default_path;
	const char* allowed_params;
	const char* path_params;
	const char* exe;
	int found;
} configuration;

static int dumper(void* user, const char* section, const char* name,
                  const char* value) {
	//static char prev_section[50] = "";
	configuration* pconfig = (configuration*)user;

	#define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
	if (strcmp(pconfig->section, section) == 0) {
		printf("%s => %s: %s [%s]\n", section, name, value, pconfig->section);

		if (MATCH(section, "default_path")) {
			pconfig->default_path = strndup(value, strlen(value));
			pconfig->found = 1;
		} else if (MATCH(section, "allowed_params")) {
			pconfig->allowed_params = strndup(value, strlen(value));
			pconfig->found = 1;
		} else if (MATCH(section, "path_params")) {
			pconfig->path_params = strndup(value, strlen(value));
			pconfig->found = 1;
		} else if (MATCH(section, "exe")) {
			pconfig->exe = strndup(value, strlen(value));
			pconfig->found = 1;
		}
	}
	// printf("search section: %s\n", (const char *) pconfig->section);

	/*
	if (strcmp(section, prev_section)) {
		printf("%s[%s]\n", (prev_section[0] ? "\n" : ""), section);
		strncpy(prev_section, section, sizeof(prev_section));
		prev_section[sizeof(prev_section) - 1] = '\0';
	}
	printf("%s = %s\n", name, value);
	*/
	return 1;
}

int main(int argc, char* argv[]) {
	int error;
	const char* empty = "";
	configuration config;
	config.section = "cmd/exe";
	config.found = 0;
	config.default_path = empty;
	config.allowed_params = empty;
	config.path_params = empty;
	config.exe = empty;
	//printf("%s\n", config.section);

	if (argc <= 1) {
		printf("Usage: ini_dump filename.ini\n");
		return 1;
	}

	error = ini_parse(argv[1], dumper, &config);
	if (error < 0) {
		printf("Can't read '%s'!\n", argv[1]);
		return 2;
	}
	else if (error) {
		printf("Bad config file (first error on line %d)!\n", error);
		return 3;
	}
	
	printf("exe:            %s\n", config.exe);
	printf("default_path:   %s\n", (config.default_path == empty) ? "(unset)" :  config.default_path);
	printf("allowed_params: %s\n", (config.allowed_params == empty) ? "(unset)" :  config.allowed_params);
	printf("path_params:    %s\n", (config.path_params == empty) ? "(unset)" :  config.path_params);

	return 0;
}
