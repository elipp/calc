#ifndef FP_T_H
#define FP_T_H

#ifdef USE_MPFR
#include <mpfr.h>
typedef mpfr_t fp_t;
#define PRECISION_BITS_INITIAL 256	// 256-bit significand

#elif LONG_DOUBLE_PRECISION
typedef long double fp_t;
#define PRECISION_BITS_INITIAL 107	// including the implicit bit. this is actually only 80 bits on x86_32 glibc, despite 128-bit data type width

#else
typedef double fp_t;
#define PRECISION_BITS_INITIAL 53

#endif 

int significant_figures_max(int precision_bits);
int minimum_bits_for_significant_figures(int significant_figures_request);

void fp_t_construct(fp_t *r);
void fp_t_destroy(fp_t *r);
void fp_t_assign(fp_t *t, const fp_t v);
void fp_t_print(fp_t f, int decimals);
long fp_t_to_long(fp_t f);
long double fp_t_to_ld(fp_t f);

int to_fp_t(fp_t *r, const char* arg, char **endptr);


#endif
