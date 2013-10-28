#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "termtree.h"
#include "string_manip.h"

int main(int argc, char* argv[]) {
	static const char* test_expr = "6/3+5-(4+5/35*((3+7)-1*3/5)*(5*33))";

	double val;
	if (!parse_mathematical_input(test_expr, &val)) {
		fprintf(stderr, "parse_mathematical_input pheyled :(\n");
		return 1;
	}

	fprintf(stderr, "expr: \"%s\" ==> result: %f\n", test_expr, val);

	static char bc_buffer[512];
	sprintf(bc_buffer, "echo \"%s\" | bc -l", test_expr);
	fprintf(stderr, "result as calculated by bc: ");
	system(bc_buffer);
	
	return 0;
}
