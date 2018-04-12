#include <stdio.h>
#include <windows.h>

int parse_wcmd(char* in, wchar_t* **args) {
	int i, c;
	size_t s = 1 + strlen(in);
	wchar_t inw[s];
	mbstowcs(inw, in, s);
	*args = CommandLineToArgvW(inw, &c);
	return c;
}

int main(int argc, char** argv) {
	printf("%ls\n", argv[1]); 
	
	int i;
	wchar_t* *a;
	int c = parse_wcmd(GetCommandLine(), &a);
	/*
	if (c)
		for (i = 0; i < c; i++) 
			printf("%d=%ls\n", i, a[i]); 
	*/
	
	wchar_t* exe = a[0];
	wchar_t* args[c];
	for (i=1; i<c; i++) {
		args[i-1] = a[i];
		printf("%d=%ls\n", i, a[i]); 
	}
	
	return 0;
}