#ifndef DEFINITIONS_H
#define DEFINITIONS_H 

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include "wlist.h"


#ifdef USE_MPFR
#include <mpfr.h>
typedef mpfr_t fp_t;
#define DEFAULT_PREC 256	// this is actually 256 bits, not digits after the decimal point

#elif LONG_DOUBLE_PRECISION
typedef long double fp_t;
#define DEFAULT_PREC 14

#else
typedef double fp_t;
#define DEFAULT_PREC 10

#endif 

#endif
