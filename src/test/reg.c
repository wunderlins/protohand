#include <stdio.h>
#include "../lib/regcpp.h"

int main(int argc, char* argv[]) {
	// reg(" +(reg)", "AAAAAAAA '$1' ", "this is a regex text");
	int ret = reg(argv[1], argv[2], argv[3]);
	if (ret != 0) {
		printf("Failed to execute regex: %d\n", ret);
	}
	return 0;
}