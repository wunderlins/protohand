#define EXTERNC extern "C"
EXTERNC int reg(const char* areg, const char* areplace, const char* str);
EXTERNC int regreplace(const char* reg, const char* str, char* result);