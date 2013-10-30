#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <math.h>
#include <stdio.h>
#include "definitions.h"

#ifdef USE_CHEM_PLUGINS
#include "chem/chem.h"
#endif

/* binary operators */

#ifdef LONG_DOUBLE_PRECISION
	#define FLOOR(f) floorl((fp_t)(f))
#else
	#define FLOOR(f) floor((fp_t)(f))
#endif

// the binary operators are stubs :P
fp_t func_binary_and(fp_t a, fp_t b);
fp_t func_binary_or(fp_t a, fp_t b);
fp_t func_factorial(fp_t a);

fp_t func_deg(fp_t rad);
fp_t func_rad(fp_t deg);


#endif

