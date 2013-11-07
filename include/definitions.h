#ifndef DEFINITIONS_H
#define DEFINITIONS_H 

#include <stdlib.h>
#include "wlist.h"

#ifdef LONG_DOUBLE_PRECISION
#define DEFAULT_PREC 12
typedef long double fp_t;
#else
#define DEFAULT_PREC 10
typedef double fp_t;
#endif 
#endif
