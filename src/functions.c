#include "functions.h"

#ifdef LONG_DOUBLE_PRECISION
	double_t (*floor_ptr)(double_t) = floorl;
	#ifdef C99_AVAILABLE
	double_t (*tgamma_ptr)(double_t) = tgammal;
	#endif
#else
	double_t (*floor_ptr)(double_t) = floor; 
	#ifdef C99_AVAILABLE
	double_t (*tgamma_ptr)(double_t) = tgamma;
	#endif
#endif

double_t func_factorial(double_t a) {
	if (floor_ptr(a) != a) {
		printf("func_factorial: error: non-integer operand!\n");
		return 0.0;
	}
	else if (a == 0) { return 1.0; }

	double_t res = 1;
	int i = a;
	while (i > 0) {
		res *= i;	
		--i;
	}
	return res;
}

double_t func_binary_and(double_t a, double_t b) { return (int)a&(int)b; }
double_t func_binary_or(double_t a, double_t b) { return (int)a|(int)b; }
double_t func_deg(double_t rad) { return (360.0L*rad)/(2*M_PI); }
double_t func_rad(double_t deg) { return (2*M_PI*deg)/360.0L; }

