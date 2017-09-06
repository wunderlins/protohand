#include <stdlib.h>
#include <ctype.h>
#include "urldecode2.h"

/*
https://stackoverflow.com/questions/2673207/c-c-url-decode-library

Example:

char *output = malloc(strlen(input)+1);
urldecode2(output, input);
printf("Decoded string: %s\n", output);
*/

void urldecode2(char *dst, const char *src) {
	char a, b;
	while (*src) {
		if ((*src == '%') &&
			((a = src[1]) && (b = src[2])) &&
			(isxdigit(a) && isxdigit(b))) {
			if (a >= 'a')
				a -= 'a'-'A';
			if (a >= 'A')
				a -= ('A' - 10);
			else
				a -= '0';
			if (b >= 'a')
				b -= 'a'-'A';
			if (b >= 'A')
				b -= ('A' - 10);
			else
				b -= '0';
			*dst++ = 16*a+b;
			src+=3;
		} else if (*src == '+') {
			*dst++ = ' ';
			src++;
		} else {
			*dst++ = *src++;
		}
	}
	*dst++ = '\0';
}