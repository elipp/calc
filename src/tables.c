#include <string.h>

#include "definitions.h"
#include "functions.h"
#include "tables.h"

#define KMP(name, fptr) { name "(", (sizeof(name)-1), fptr }

#ifdef LONG_DOUBLE_PRECISION

const key_mathfuncptr_pair functions[] = { 
	KMP("cos", cosl),
	KMP("sin", sinl),
	KMP("tan", tanl),
        KMP("acos", acosl), 
	KMP("asin", asinl), 
	KMP("atan", atanl),
	KMP("exp", expl), 
	KMP("ln", logl), 
	KMP("log", log10l),
	KMP("sqrt", sqrtl), 
	KMP("abs", fabsl), 
	KMP("cosh", coshl),
	KMP("sinh", sinhl), 
	KMP("tanh", tanhl), 
	KMP("acosh", acoshl), 
	KMP("asinh", asinhl), 
	KMP("atanh", atanhl), 
	KMP("floor", floorl),
	KMP("ceil", ceill),
	KMP("deg", func_deg), 
	KMP("rad", func_rad)

	#ifdef C99_AVAILABLE 
	, KMP("gamma", tgammal)
	#endif

};

const key_constant_pair constants[] = { { "pi",  3.14159265358979323846264338327950288L },
					{ "Pi",  3.14159265358979323846264338327950288L },
					{ "PI",  3.14159265358979323846264338327950288L },
					{ "e",   2.71828182845904523536028747135266249L },
					{ "c",	 299792458L },
					{ "h", 	 6.62606957e-34L },
					{ "m_e", 9.10938291e-31L },
					{ "m_p", 1.672621777e-27L },
					{ "m_n", 1.674927351e-27L },
					{ "m_u", 1.660538921e-27L },
					{ "N_A", 6.02214129e23L },
					{ "R", 8.3144621L }

};

#else

const key_mathfuncptr_pair functions[] = { 
	KMP("cos", cos),
	KMP("sin", sin),
	KMP("tan", tan),
        KMP("acos", acos), 
	KMP("asin", asin), 
	KMP("atan", atan),
	KMP("exp", exp), 
	KMP("ln", log), 
	KMP("log", log10),
	KMP("sqrt", sqrt), 
	KMP("abs", fabs), 
	KMP("cosh", cosh),
	KMP("sinh", sinh), 
	KMP("tanh", tanh), 
	KMP("acosh", acosh), 
	KMP("asinh", asinh), 
	KMP("atanh", atanh), 
	KMP("floor", floor),
	KMP("ceil", ceil),
	KMP("deg", func_deg),
	KMP("rad", func_rad)

	#ifdef C99_AVAILABLE 
	, KMP("gamma", tgamma)
	#endif

};

const key_constant_pair constants[] = { { "pi",  3.14159265358979323846264338327950288L },
					{ "Pi",  3.14159265358979323846264338327950288L },
					{ "PI",  3.14159265358979323846264338327950288L },
					{ "e",   2.71828182845904523536028747135266249L },
					{ "c",	 299792458L },
					{ "h", 	 6.62606957e-34L },
					{ "m_e", 9.10938291e-31L },
					{ "m_p", 1.672621777e-27L },
					{ "m_n", 1.674927351e-27L },
					{ "m_u", 1.660538921e-27L },
					{ "N_A", 6.02214129e23L },
					{ "R", 8.3144621L }

};




#endif


const size_t functions_table_size = sizeof(functions)/sizeof(functions[0]);
const size_t constants_table_size = sizeof(constants)/sizeof(constants[0]);

#ifdef USE_CHEM_PLUGINS
const key_strfuncptr_pair chem_functions[] = { "molarmass", func_molar_mass };
const size_t chem_functions_table_size = sizeof(chem_functions)/sizeof(chem_functions[0]);
#endif

int clashes_with_predefined(const char* arg) {
	int i = 0;

	while (i < constants_table_size) {
		if (strcmp(constants[i].key, arg) == 0) {
			printf("my: error: operand varname clashes with predefined \"%s\" (= %f)\n", 
				constants[i].key, (double)constants[i].value);
			return 1;
		}
		++i;
	}
	return 0;
}


