#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <math.h>
#include <stdio.h>
#include "definitions.h"

#ifdef USE_CHEM_PLUGINS
#include "chem/chem.h"
#endif

/* binary operators */

// the binary operators are stubs :P
double_t func_binary_and(double_t a, double_t b);
double_t func_binary_or(double_t a, double_t b);
double_t func_factorial(double_t a);

double_t func_deg(double_t rad);
double_t func_rad(double_t deg);


#endif

