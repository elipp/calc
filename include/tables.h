#include "definitions.h"

extern const key_mathfuncptr_pair functions[];
extern const key_constant_pair constants[];

extern const size_t functions_table_size;
extern const size_t constants_table_size; 

#ifdef USE_CHEM_PLUGINS
extern const key_strfuncptr_pair chem_functions[]; 
extern const size_t chem_functions_table_size;
#endif

int clashes_with_predefined(const char* arg); 
