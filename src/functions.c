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

// no ! or binary operators (&, |, ^) yet
const char valid_math_chars[] = { '(', ')', '.', '+', '-', '*', '/', '^', '%', '_', '\0' }; 

int valid_math_char(char c) {
	int i = 0;
	int found = 0;
	for (; i < sizeof(valid_math_chars); ++i) {
		if (c == valid_math_chars[i]) {
			found = 1;
			break;
		}
	}

	return found;
}

fp_t f_add(fp_t a, fp_t b) { return a + b; }
fp_t f_sub(fp_t a, fp_t b) { return a - b; }
fp_t f_mul(fp_t a, fp_t b) { return a * b; }
fp_t f_div(fp_t a, fp_t b) { return a / b; }
fp_t f_pow(fp_t a, fp_t b) { return POW(a, b); }
fp_t f_mod(fp_t a, fp_t b) { return (long long)a%(long long)b; }

fp_t func_binary_and(fp_t a, fp_t b) { return (int)a&(int)b; }
fp_t func_binary_or(fp_t a, fp_t b) { return (int)a|(int)b; }
fp_t func_deg(fp_t rad) { return (360.0L*rad)/(2*M_PI); }
fp_t func_rad(fp_t deg) { return (2*M_PI*deg)/360.0L; }

