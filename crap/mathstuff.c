#include "mathstuff.h"

// no ! or binary operators (&, |, ^) yet
const char valid_math_chars[] = { '(', ')', '.', '+', '-', '*', '/', '^', '%', '\0' }; 

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

double f_add(double a, double b) { return a + b; }
double f_sub(double a, double b) { return a - b; }
double f_mul(double a, double b) { return a * b; }
double f_div(double a, double b) { return a / b; }
double f_pow(double a, double b) { return pow(a, b); }
double f_mod(double a, double b) { return (long long)a%(long long)b; }
