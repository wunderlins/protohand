#include <stdio.h>
#include <string.h>
#include "lib/errstr.h"

#define BLOCK_SIZE (1024*200)
extern char* errstr[255];

int main() {
	define_error_messages();
	int i = 0;
	
	//int pos = 0;
	char* buffer = malloc(sizeof(char*) * BLOCK_SIZE);
	//int buffer_s = BLOCK_SIZE;
	char *linebuffer = malloc(sizeof(char*) * BLOCK_SIZE);
	char *template = malloc(sizeof(char*) * BLOCK_SIZE);
	template[0] = 0;
	buffer[0] = 0;
	linebuffer[0] = 0;
		
	// open error_template.html
	FILE* fp;
	fp = fopen("error_template.html", "r");
	//fwrite(usage_str , sizeof(char), strlen(usage_str), fp);
	while(fgets(linebuffer, 255, (FILE*) fp)) {
		//printf("%s\n", linebuffer);
		strcat(template, linebuffer);
	}
	fclose(fp);
	//printf("%s\n", template);
	
	// find ERROR_MESSAGES
	char *result = strstr(template, "ERROR_MESSAGES");
	int position = result - template;
	//printf("Keyword found at %d\n", position);
	//int substringLength = strlen(template) - position;
	
	// copy the beginning of the string to new buffer
	strncpy(buffer, template, position);
	
	// create error messages string and copy error messages string to 
	// new buffer
	for(i=0; i<255; i++) {
		if (errstr[i] == NULL)
			continue;
		// printf("%d %s\n", i, errstr[i]);
		sprintf(linebuffer, "\t\t<div class=\"detail\" id=\"err%d\">%s</div>\n",
		                    i, errstr[i]);
		strcat(buffer, linebuffer);
	}
	
	// copy remaining string to buffer
	strcat(buffer, template+14+position);
	//printf("%s\n", buffer);
	
	// save buffer to error.html
	fp = fopen("generated/error.html", "wb");
	fwrite(buffer , sizeof(char), strlen(buffer), fp);
	fclose(fp);
	
	return 0;
}