#include <iostream>
#include <string>
#include <regex> // std::regex
#include <pcrecpp.h> // pcrecpp::RE -- needs "-lpcrecpp -lpcre"

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

#ifdef CPPMAIN
int main() {
	const char* regex   = "regex";
	const char* replace = "replace";
	const char* str = "asdf asdf asdf asdf regex sdf asdfasdf";
	reg(regex, replace, str);
    return 0;
}
#endif