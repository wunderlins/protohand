#include <iostream>
#include <string>
// #include <regex> // std::regex
// #include <pcrecpp.h> // pcrecpp::RE -- needs "-lpcrecpp -lpcre"

//#ifdef __cplusplus
#define EXTERNC extern "C"
//#else
//#define EXTERNC
//#endif

using namespace std;

EXTERNC int reg(const char* areg, const char* areplace) {
	string reg(areg);
	string replace(areplace);
	cout << reg << " " << replace << endl;
	return 0;
}

#ifdef CPPMAIN
int main() {
	const char* regex   = "regex";
	const char* replace = "replace";
	reg(regex, replace);
    return 0;
}
#endif