#include "functions.h"
fp_t func_factorial(fp_t a) {
	if (FLOOR(a) != a) {
		printf("func_factorial: error: non-integer operand!\n");
		return 0.0;
	}
	else if (a == 0) { return 1.0; }

	fp_t res = 1;
	int i = a;
	while (i > 0) {
		res *= i;	
		--i;
	}
	return res;
}

fp_t func_binary_and(fp_t a, fp_t b) { return (int)a&(int)b; }
fp_t func_binary_or(fp_t a, fp_t b) { return (int)a|(int)b; }
fp_t func_deg(fp_t rad) { return (360.0L*rad)/(2*M_PI); }
fp_t func_rad(fp_t deg) { return (2*M_PI*deg)/360.0L; }

