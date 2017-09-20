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
	std::regex re(areg);
	string reg(areg);
	string replace(areplace);
	cout << reg << " " << replace << endl;
	string res = std::regex_replace(str, re, replace);
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