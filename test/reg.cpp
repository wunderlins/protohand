#include <iostream>
#include <string>
#include <regex> // std::regex
#include <pcrecpp.h> // pcrecpp::RE -- needs "-lpcrecpp -lpcre"
#include <string.h>

//#ifdef __cplusplus
#define EXTERNC extern "C"
//#else
//#define EXTERNC
//#endif

using namespace std;

// Currently, the following modifiers are supported
//
//    modifier              description               Perl corresponding
//
//    PCRE_CASELESS         case insensitive match    /i
//    PCRE_MULTILINE        multiple lines match      /m
//    PCRE_DOTALL           dot matches newlines      /s
//    PCRE_DOLLAR_ENDONLY   $ matches only at end     N/A
//    PCRE_EXTRA            strict escape parsing     N/A
//    PCRE_EXTENDED         ignore whitespaces        /x
//    PCRE_UTF8             handles UTF8 chars        built-in
//    PCRE_UNGREEDY         reverses * and *?         N/A
//    PCRE_NO_AUTO_CAPTURE  disables matching parens  N/A (*)
//
// TODO: implement re options
// TODO: implement parser that takes a regex string like /re/replace/modifiers

int reg_error_code = 0;
EXTERNC int reg(const char* areg, const char* areplace, const char* str) {
	string res = "";
	string replace(areplace);
	
	try {
		regex re(areg);
		cout << areg << " " << areplace << endl;
		res = regex_replace(str, re, areplace);
	} catch (regex_error e) {
		reg_error_code = e.code();
		return reg_error_code;
	}
	
	cout << "Result: " << res << endl;
	return 0;
}

/**
 * regex replace
 *
 * param reg takes a perl compatible regex in the form of 
 * /search/replace/modifiers, the delimiting character is always '/'.
 * '/' within search and replace must be backslash escaped ('\/').
 * 
 * the result string will be allocated in this function because 
 * the caller cannot know the required size for the result in advance.
 * 
 * returns 0 on success or an regcpp.h error code otherwise.
 */
EXTERNC int regreplace(const char* areg, const char* str, char* result) {
	// parse the regex string
	char *token;
	int i, l, pos, len = 0;
	len = strlen(areg);
	
	char* pre      = (char*) malloc(sizeof(char*) * (len+1));
	char* reg      = (char*) malloc(sizeof(char*) * (len+1));
	char* replace  = (char*) malloc(sizeof(char*) * (len+1));
	char* modifier = (char*) malloc(sizeof(char*) * (len+1));
	
	pre[0]      = '\0'; // init empty string
	reg[0]      = '\0'; // init empty string
	replace[0]  = '\0'; // init empty string
	modifier[0] = '\0'; // init empty string
	
	char* parts[5] = {pre, reg, replace, modifier, NULL};
	
	for (l=0, i=0; i<len && pos < 5; i++) {
		
		if (pos > 4) {
			printf("Too many parts\n");
			break;			
		}
		
		if(areg[i] == '/' && i == 0) { // slash at the beginning
			pos++;
			l = 0;
			printf("first at %d\n", i);
			continue;
		}
		
		if(areg[i] == '\\' && areg[i+1] == '/') {
			pos--;
			continue;
		}
		
		if(areg[i] == '\\' && areg[i+1] == '\\') {
			continue;
		}
		
		if(areg[i] == '/') {
			pos++;
			l = 0;
			printf("%d at %d\n", pos, i);
			continue;
		}
		
		printf("i %c, pos %d l: %d\n", areg[i], pos, l);
		parts[pos][l++] = areg[i];
		parts[pos][l] = '\0';
		//printf("pos: %d, l %d, i %d\n", pos, l, i);
	}
	//pre[0] = '1'; pre[1] = 0;
	//parts[1][0] = '1'; parts[1][1] = 0;
	
	printf("\n\
pre:      %s\n\
reg:      %s\n\
replace:  %s\n\
modifier: %s\n", pre, reg, replace, modifier);

}

#ifdef CPPMAIN
int main() {
	/*
	const char* regex   = "regex";
	const char* replace = "replace";
	const char* str = "asdf asdf asdf asdf regex sdf asdfasdf";
	reg(regex, replace, str);
	*/
	
	char* result;
	const char* reg = "/search/\\/repl\\\\ace/m";
	
	int ret = regreplace(reg, "asb defg 100 sdfsdf", result);
	
    return 0;
}
#endif