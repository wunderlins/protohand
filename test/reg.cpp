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

EXTERNC int reg(const char* areg, const char* areplace, const char* str) {
	string res = "";
	string reg(areg);
	string replace(areplace);
	
	try {
		std::regex re(areg);
		cout << reg << " " << replace << endl;
		res = regex_replace(str, re, replace);
	} catch (regex_error e) {
		return e.code();
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