#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <math.h>
#include <stdio.h>
#include "fp_t.h"

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

int valid_math_char(char c);

typedef void (*mathfuncptr)(fp_t*, fp_t, fp_t);

void f_add(fp_t* r, fp_t a, fp_t b);
void f_sub(fp_t *r, fp_t a, fp_t b);
void f_mul(fp_t *r, fp_t a, fp_t b);
void f_div(fp_t *r, fp_t a, fp_t b);
void f_pow(fp_t *r, fp_t a, fp_t b);
void f_mod(fp_t *r, fp_t a, fp_t b);

void func_binary_and(fp_t *r, fp_t a, fp_t b);
void func_binary_or(fp_t *r, fp_t a, fp_t b);
void func_factorial(fp_t *r, fp_t f);

void func_deg(fp_t *r, fp_t rad);
void func_rad(fp_t *r, fp_t deg);

#endif

