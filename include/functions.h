#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <math.h>
#include <stdio.h>
#include "definitions.h"

#ifdef USE_CHEM_PLUGINS
#include "chem/chem.h"
#endif

#ifdef LONG_DOUBLE_PRECISION
	#define FLOOR(x) floorl((x))
	#define POW(a, b) powl((a), (b))
#else
	#define FLOOR(f) floor((f))
	#define POW(a, b) pow((a), (b))
#endif

// the binary operators are stubs :P

extern const char valid_math_chars[];
int valid_math_char(char c);

typedef fp_t (*mathfuncptr)(fp_t, fp_t);

fp_t f_add(fp_t a, fp_t b);
fp_t f_sub(fp_t a, fp_t b);
fp_t f_mul(fp_t a, fp_t b);
fp_t f_div(fp_t a, fp_t b);
fp_t f_pow(fp_t a, fp_t b);
fp_t f_mod(fp_t a, fp_t b);


fp_t func_binary_and(fp_t a, fp_t b);
fp_t func_binary_or(fp_t a, fp_t b);
fp_t func_factorial(fp_t a);

fp_t func_deg(fp_t rad);
fp_t func_rad(fp_t deg);


#endif

