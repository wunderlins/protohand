#ifdef __cplusplus
	extern "C" int reg(const char* areg, const char* areplace, const char* str);
	extern "C" const char* regreplace(const char* reg, const char* str);
#else
	int reg(const char* areg, const char* areplace, const char* str);
	const char* regreplace(const char* reg, const char* str);
#endif

extern int regerrno;