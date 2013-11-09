#include <string.h>
#include <stdlib.h>

#include "fp_t.h"
#include "functions.h"
#include "tables.h"
#include "commands.h"


#ifdef USE_MPFR
#define MATH_WRAPPER(funcname) void cw_##funcname(fp_t *r, fp_t arg) { mpfr_##funcname(*r, arg, MPFR_RNDN); }
#define MATH_WRAPPER_EXPLICIT(mpfr_name, funcname) void cw_##funcname(fp_t *r, fp_t arg) { mpfr_##mpfr_name(*r, arg, MPFR_RNDN); }
#else

#ifdef LONG_DOUBLE_PRECISION
#define MATH_WRAPPER(funcname) void cw_##funcname(fp_t *r, fp_t arg) { *r = funcname##l(arg); }
#else
#define MATH_WRAPPER(funcname) void cw_##funcname(fp_t *r, fp_t arg) { *r = funcname(arg); }
#endif // LONG_DOUBLE_PRECISION

#endif // USE_MPFR

MATH_WRAPPER(cos);
MATH_WRAPPER(sin);
MATH_WRAPPER(tan);
MATH_WRAPPER(acos);
MATH_WRAPPER(asin);
MATH_WRAPPER(atan);
MATH_WRAPPER(exp);
MATH_WRAPPER(log);
MATH_WRAPPER(log2);
MATH_WRAPPER(log10);
MATH_WRAPPER(sqrt);
MATH_WRAPPER(cosh);
MATH_WRAPPER(sinh);
MATH_WRAPPER(tanh);
MATH_WRAPPER(asinh);
MATH_WRAPPER(acosh);
MATH_WRAPPER(atanh);

#ifdef USE_MPFR
MATH_WRAPPER_EXPLICIT(abs, fabs);
MATH_WRAPPER_EXPLICIT(rint_floor, floor);
MATH_WRAPPER_EXPLICIT(rint_ceil, ceil);
MATH_WRAPPER_EXPLICIT(gamma, tgamma);

#else
MATH_WRAPPER(fabs);
MATH_WRAPPER(floor);
MATH_WRAPPER(ceil);
MATH_WRAPPER(tgamma);

#endif

#define KMPSTD(namestr, funcname) { namestr "(", (sizeof(namestr)-1), cw_##funcname}
#define KMP(namestr, funcname) { namestr "(", (sizeof(namestr)-1), funcname}

const key_mathfuncptr_pair functions[] = { 
	KMPSTD("sqrt", sqrt),
	KMPSTD("cos", cos),
	KMPSTD("sin", sin),
	KMPSTD("tan", tan),
        KMPSTD("acos", acos), 
	KMPSTD("asin", asin), 
	KMPSTD("atan", atan),
	KMPSTD("exp", exp), 
	KMPSTD("ln", log), 
	KMPSTD("log2", log2), 
	KMPSTD("log10", log10),
	KMPSTD("sqrt", sqrt), 
	KMPSTD("abs", fabs), 
	KMPSTD("cosh", cosh),
	KMPSTD("sinh", sinh), 
	KMPSTD("tanh", tanh), 
	KMPSTD("acosh", acosh), 
	KMPSTD("asinh", asinh), 
	KMPSTD("atanh", atanh), 
	KMPSTD("floor", floor),
	KMPSTD("ceil", ceil),
	KMPSTD("gamma", tgamma),
	KMP("deg", func_deg), 
	KMP("rad", func_rad),
	KMP("fact", func_factorial)
};

key_constant_pair *constants = NULL;

static void constant_construct(key_constant_pair *k, const char* id, const char* valstring) {
	k->key = strdup(id);
	char *dummy;
	if (!to_fp_t(&k->value, valstring, &dummy)) fprintf(stderr, "warning: constant_construct: to_fp_t failed!\n");
}

const size_t functions_table_size = sizeof(functions)/sizeof(functions[0]);
const size_t constants_table_size = 12;

void setup_constants() {
	if (!constants) constants = malloc(constants_table_size*sizeof(key_constant_pair));

	int i = 0;
	for (; i < constants_table_size; ++i) { fp_t_construct(&constants[i].value); }

	#ifdef USE_MPFR
	mpfr_t pi, e;
	mpfr_inits2(precision_bits, pi, e, (mpfr_ptr)0);

	mpfr_const_pi(pi, MPFR_RNDN);
	mpfr_set_str(e, "2.71828182845904523536028747135266249775724709369995957496696762772407663\
	0353547594571382178525166427427466391932003059921817413596629043572900334\
	295260595630738132328627943490763233829880753195251019", 0, MPFR_RNDN);
	
	fp_t_assign(&constants[0].value, pi);
	constants[0].key = strdup("pi");
	fp_t_assign(&constants[1].value, pi);
	constants[1].key = strdup("Pi");
	fp_t_assign(&constants[2].value, pi);
	constants[2].key = strdup("PI");
	fp_t_assign(&constants[3].value, e);
	constants[3].key = strdup("e");

	#else
	constant_construct(&constants[0], "pi", "3.14159265358979323846264338327950288");
	constant_construct(&constants[1], "Pi", "3.14159265358979323846264338327950288");
	constant_construct(&constants[2], "PI", "3.14159265358979323846264338327950288");
	constant_construct(&constants[3], "e", "2.71828182845904523536028747135266250");
	#endif

	constant_construct(&constants[4], "c", "299792458");
	constant_construct(&constants[5], "h", "6.62607e-34");
	constant_construct(&constants[6], "m_e", "9.10938291e-31");
	constant_construct(&constants[7], "m_p", "1.672621777e-27");
	constant_construct(&constants[8], "m_n", "1.674927351e-27");
	constant_construct(&constants[9], "m_u", "1.660538921e-27");
	constant_construct(&constants[10], "N_A", "6.02214129e23");
	constant_construct(&constants[11], "R", "8.3144621");
}

void clean_constants() {
	int i = 0;
	for (; i < constants_table_size; ++i) {
	#ifdef USE_MPFR
		mpfr_clear(constants[i].value);
	#endif
		free(constants[i].key);
	}

}

#ifdef USE_CHEM_PLUGINS
const key_strfuncptr_pair chem_functions[] = { "molarmass", func_molar_mass };
const size_t chem_functions_table_size = sizeof(chem_functions)/sizeof(chem_functions[0]);
#endif

int clashes_with_predefined(const char* arg) {
	int i = 0;

	while (i < constants_table_size) {
		if (strcmp(constants[i].key, arg) == 0) {
			printf("my: error: operand varname clashes with predefined \"%s\" (value ", constants[i].key);
			fp_t_print(constants[i].value, print_significant_figures);
			puts(")\n");		
			return 1;
		}
		++i;
	}
	return 0;
}


