#ifndef TABLES_H
#define TABLES_H

#include "fp_t.h"

typedef struct {
	const char* key;
	size_t key_len;
	void (*funcptr)(fp_t*, fp_t);
} key_mathfuncptr_pair;

typedef struct {
	char* key;
	fp_t value;
} key_constant_pair;

extern const key_mathfuncptr_pair functions[];
extern key_constant_pair *constants;

extern const size_t functions_table_size;
extern const size_t constants_table_size; 

#ifdef USE_CHEM_PLUGINS
extern const key_strfuncptr_pair chem_functions[]; 
extern const size_t chem_functions_table_size;
#endif

int clashes_with_predefined(const char* arg); 
void setup_constants();
void clean_constants();

#endif
