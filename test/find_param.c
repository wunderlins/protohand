#include <stdio.h>
#include <string.h>
#include "../stringlib.h"

int main(int argc, char** argv) {
	int i=0;
	char* search = "--path = 	C:\\windows\\CSUP.txt";
	//char* search = "--help";
	char* options[] = {"--help", "--search", "--path", "--whatever", "whatever--path"};
	
	// create a strng str_array from string literals
	struct str_array a_test = str_array("a", "bb", "ccccc", "d", "e", "f");
	
	// create a string str_array from pointers 
	struct str_array a_options = str_array_make(options, 5);
	
	// test literal str_array
	printf("a_test length: %d\n", a_test.length);
	for(i=0; i<a_test.length; i++) {
		printf("--> a_test[%d]: %s\n", i, a_test.items[i]);
	}
	
	// test pointer str_array
	printf("a_options length: %d\n", a_options.length);
	for(i=0; i<a_options.length; i++) {
		printf("--> a_options[%d]: %s\n", i, a_options.items[i]);
	}
	
	int result = 0;
	result = find_param(search, &a_options);
	
	printf("Result: %d\n", result);
	
	return 0;
}