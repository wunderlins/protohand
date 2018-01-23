#include "transform.h"

int transform_error;

/**
 * left pad a string with zeros. 
 *
 * the string shall be 9 digits long when transformed.
 * reject input strings that are longer than maxlen chars.
 *
 * the user must make sure to allocate enough memory in strout to hold the 
 * resulting string which is maxlen + 1.
 */
int transform_lpadzero(char* strin, char* strout, int maxlen) {
	transform_error = 0;
	strout[0] = 0;
	
	// out len max, nothing to pad
	int l = strlen(strin);
	if (l == maxlen) {
		strcpy(strout, strin);
		return 0;
	}
	
	if (l > maxlen) {
		transform_error = 1;
		return -1;
	}
	
	int i=0;
	for (i=0; i<maxlen-l; i++)
		strout[i] = '0';
	strout[i] = 0;
	strcat(strout, strin);
	
	return 0;
}

/**
 * remove all leading zeros.
 *
 * make sure that the last zero is preserved if the input 
 * string consists of zeros only.
 *
 * the user must make sure to allocate enough memory in strout to hold the 
 * resulting string which at least as long as strin but possibly shorter.
 */
int transform_ltrimzero(char* strin, char* strout) {
	transform_error = 0;
	strout[0] = 0;
	
	while (strin[0] == '0') {
		strin++;
		
		if (strlen(strin) == 1)
			break;
	}
	
	strcpy(strout, strin);
	
	return 0;
}

#ifdef TRANSFORM_MAIN
int main(int argc, char** argv) {
	
	int ret = 0;
	int r = 0;
	
	char* str = "123";
	char res[10] = "";
	
	// == test lpadzero
	
	// valid
	r = transform_lpadzero(str, res, 9);
	printf("%s, %s, %d\n", res, str, r);
	if (r != 0) ret++;
	
	// no padding
	r = transform_lpadzero("123123123", res, 9);
	printf("%s, %s, %d\n", res, "123123123", r);
	if (r != 0) ret++;
	
	// error
	r = transform_lpadzero("1231231231", res, 9);
	printf("%s, %s, %d\n", res, "1231231231", r);
	if (r != -1) ret++;
	
	
	// == test ltrimzero
	r = transform_ltrimzero("0000001", res);
	printf("%s, %s, %d\n", res, "0000001", r);
	if (r != 0) ret++;
	
	r = transform_ltrimzero("0000000", res);
	printf("%s, %s, %d\n", res, "0000000", r);
	if (r != 0) ret++;
	
	r = transform_ltrimzero("10000000", res);
	printf("%s, %s, %d\n", res, "10000000", r);
	if (r != 0) ret++;
	
	return ret;
}
#endif