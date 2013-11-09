#include <string.h>
#include "functions.h"
#include "commands.h"
#include "definitions.h"

int valid_math_char(char c) {
	static const char valid_math_chars[] = { '(', ')', '.', '+', '-', '*', '/', '^', '%', '_', '\0' }; 
	int i = 0;
	int found = 0;
	for (; i < sizeof(valid_math_chars); ++i) {
		if (c == valid_math_chars[i]) {
			found = 1;
			break;
		}
	}
	return found;
}

#ifndef USE_MPFR
void func_factorial(fp_t *r, fp_t a) {
	
	if ((long)a == 0) { *r = 1; return; }

	int i = 2;
	*r = 1;
	while (i <= (long)a) {
		*r *= i;	
		++i;
	}
}

// no ! or binary operators (&, |, ^) yet


void f_add(fp_t *r, fp_t a, fp_t b) { *r = a + b; }
void f_sub(fp_t *r, fp_t a, fp_t b) { *r = a - b; }
void f_mul(fp_t *r, fp_t a, fp_t b) { *r = a * b; }
void f_div(fp_t *r, fp_t a, fp_t b) { *r = a / b; }
void f_pow(fp_t *r, fp_t a, fp_t b) { *r = POW(a, b); }
void f_mod(fp_t *r, fp_t a, fp_t b) { *r = (long long)a%(long long)b; }

void func_binary_and(fp_t *r, fp_t a, fp_t b) { *r= (long long)a&(long long)b; }
void func_binary_or(fp_t *r, fp_t a, fp_t b) { *r = (long long)a|(long long)b; }
void func_deg(fp_t *r, fp_t rad) { *r = (360.0L*rad)/(2*M_PI); }
void func_rad(fp_t *r, fp_t deg) { *r = (2*M_PI*deg)/360.0L; }

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


#else // USE_MPFR 


void f_add(fp_t *r, fp_t a, fp_t b) { 
	mpfr_add(*r, a, b, MPFR_RNDN);
}

void f_sub(fp_t *r, fp_t a, fp_t b) { 
	mpfr_sub(*r, a, b, MPFR_RNDN);
}

void f_mul(fp_t *r, fp_t a, fp_t b) { 
	mpfr_mul(*r, a, b, MPFR_RNDN);
}

void f_div(fp_t *r, fp_t a, fp_t b) { 
	mpfr_div(*r, a, b, MPFR_RNDN);
}

void f_pow(fp_t *r, fp_t a, fp_t b) { 
	mpfr_pow(*r, a, b, MPFR_RNDN);
}

void f_mod(fp_t *r, fp_t a, fp_t b) {
	mpfr_set_ui(*r, 0, MPFR_RNDN);
}

void func_deg(fp_t *r, fp_t rad) { 
	mpfr_t twopi;
	mpfr_inits2(precision, twopi, *r, (mpfr_ptr)0);
	mpfr_mul_ui(*r, rad, 360, MPFR_RNDN);
	mpfr_const_pi(twopi, MPFR_RNDN);
	mpfr_mul_ui(twopi, twopi, 2, MPFR_RNDN);
	mpfr_div(*r, *r, twopi, MPFR_RNDN);
	mpfr_clear(twopi);
}

void func_rad(fp_t *r, fp_t deg) { 
	mpfr_t twopi;
	mpfr_inits2(precision, twopi, *r, (mpfr_ptr)0);
	mpfr_const_pi(twopi, MPFR_RNDN);
	mpfr_mul(*r, deg, twopi, MPFR_RNDN);
	mpfr_div_ui(*r, *r, 360, MPFR_RNDN);
	mpfr_clear(twopi);
}

void func_factorial(fp_t* v, fp_t f) {
	mpfr_fac_ui(*v, fp_t_to_long(f), MPFR_RNDN);
}

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

void fp_t_print(fp_t f, int precision_bits) {
	char fmt_buffer[64];

	if (precision_bits == 0) sprintf(fmt_buffer, "%%.0Rf");
	else { 
		int precision_digits = precision_bits * (log(2)/log(10)) - 1;
		sprintf(fmt_buffer, "%%.%dRg", precision_digits);
	}

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

#endif
