#ifndef MATHSTUFF_H
#define MATHSTUFF_H

#include <math.h>

extern const char valid_math_chars[];
int valid_math_char(char c);

typedef double (*mathfuncptr)(double, double);
double f_add(double a, double b);
double f_sub(double a, double b);
double f_mul(double a, double b);
double f_div(double a, double b);
double f_pow(double a, double b);
double f_mod(double a, double b);

#endif
