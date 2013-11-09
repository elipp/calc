#include <stdio.h>

#include "definitions.h"
#include "termtree.h"
#include "string_allocator.h"

int main(int argc, char* argv[]) {

	static const char* expr = "sin(0.3*pi^3)^3*2  / 31.31+531*(-999*33.25/833-1*e*log(20/tan(20)) - (e/pi+1)^2/3.33)/(301.3+53*sin(31*cos(445)))";

	while (1) {
		fp_t r;
		parse_mathematical_input(expr, &r);
		sa_clearbuf();
	}

	return 0;
}
