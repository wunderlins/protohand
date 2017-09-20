#include <stdio.h>
#include "regcpp.h"

int main(int argc, char* argv[]) {
	// reg(" +(reg)", "AAAAAAAA '$1' ", "this is a regex text");
	int ret = reg(argv[1], argv[2], argv[3]);
	return 0;
}