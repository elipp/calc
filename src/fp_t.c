#include <stdlib.h>
#include <stdio.h>
#include "fp_t.h"
#include "commands.h"

static const long double significant_f_coeff = 0.30102999566398119521L;	// log_n(2)/log_n(10)
static const long double bit_coeff = 1.0/0.30102999566398119521L;

int significant_figures_max(int precision_bits) {
	int precision_digits = precision_bits * significant_f_coeff; // at maximum
	return precision_digits;
}

int minimum_bits_for_significant_figures(int significant_figures_request) {
	int bits_required_minimum = significant_figures_request * bit_coeff;
	return bits_required_minimum;
}

#ifdef USE_MPFR

void fp_t_construct(fp_t *r) {
	mpfr_init(*r);	// initialize to default precision (mpfr_set_precision)
	mpfr_set_ui(*r, 0, MPFR_RNDZ);
}

void fp_t_destroy(fp_t *r) {
	mpfr_clear(*r);
}

void fp_t_assign(fp_t *r, const fp_t v) {
	mpfr_set(*r, v, MPFR_RNDN);
}

void fp_t_print(fp_t f, int significant_figures) {
	char fmt_buffer[64];
	sprintf(fmt_buffer, "%%.%dRg", significant_figures);

	mpfr_printf(fmt_buffer, f);
}

long fp_t_to_long(fp_t f) {
	return mpfr_get_si(f, MPFR_RNDN);	
}

long double fp_t_to_ld(fp_t f) {
	return mpfr_get_ld(f, MPFR_RNDN);
}

int to_fp_t(fp_t *r, const char* arg, char **endptr) { 
	mpfr_strtofr(*r, arg, endptr, 0, MPFR_RNDN);
	return (*endptr == arg+strlen(arg));
}

#else
void fp_t_construct(fp_t *r) { *r = 0; }
void fp_t_destroy(fp_t *r) { *r = 0; }

void fp_t_print(fp_t f, int precision_figures) {
	char fmt_buffer[64];
	sprintf(fmt_buffer, "%%.%dLg", precision_figures);
	printf(fmt_buffer, f);
}

long fp_t_to_long(fp_t f) {
	return (long)f;
}

long double fp_t_to_ld(fp_t f) {
	return f;
}

int to_fp_t(fp_t *r, const char* arg, char **endptr) {
#ifdef LONG_DOUBLE_PRECISION
	*r = strtold(arg, endptr);
#else
	*r = strtod(arg, endptr);
#endif
	return (*endptr == arg + strlen(arg));
}

void fp_t_assign(fp_t *r, const fp_t f) {
	*r = f;
}

#endif


