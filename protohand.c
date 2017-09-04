/**
 * Read string from stdinput nd parse uri parts [authority,path,query].
 * for a detailed URI spec see: https://tools.ietf.org/html/rfc3986
 * (See section 3 for a quick start about schemes).
 *
 * `scheme` is used by the os to invoke this program.
 * The program extracts then authority and maps it to an executable (external mapping).
 * FIXME: the `path`part can be useful but it's unclear for what yet.
 * the `query` part shall hold all program parameters. PAthis in the parameters must be checked and sanitized. The query string must be a properly urlencoded associtative array of program parameters in the form of:
 *   windows: ?/a&/b=1 which translates into "/a /b=1" command line parameters
 *   *nix:    ?-y&--long-param=fubar which translates into: "-y --long-param=fubar"
 *   attributes: ?param1&param2 which translates into "param1 param2"
 * all schemes above can be mxed since it is up to the called program to interpret the commandline arguments and therefore the called program's specs must be met.
 *
 * 2017, Simon Wunderlin <swunderlin@gmail.com>
 */

#include <stdio.h>
#include <string.h>

// maximum input length from stdin, prevent buffer overflows
#define STDIN_MAX 1024

// Error codes, used as return codes
#define OK       0
#define NO_INPUT 1
#define TOO_LONG 2

// debug levels, 0=off, 1=on
#define DEBUG 1

int main(int argc, char** argv) {
	
	char buff[STDIN_MAX];
	size_t sz = sizeof(buff);
	int ch, extra;
	
	// read data from stdin. no data? return error.
	if (fgets (buff, sz, stdin) == NULL)
		return NO_INPUT;
    
	// If it was too long, there'll be no newline. In that case, we flush
	// to end of line so that excess doesn't affect the next call.
	if (buff[strlen(buff)-1] != '\n') {
		extra = 0;
		while (((ch = getchar()) != '\n') && (ch != EOF))
			extra = 1;
		
		if (extra == 1)
			return TOO_LONG;
	}
	
	#if DEBUG == 1
	printf("%s", buff);
	#endif
	
	return OK;
}