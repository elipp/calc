#ifndef DEFINITIONS_H
#define DEFINITIONS_H 

#include <stdlib.h>

#ifdef LONG_DOUBLE_PRECISION
#define DEFAULT_PREC 16
typedef long double fp_t;
#else
#define DEFAULT_PREC 10
typedef double fp_t;
#endif 

typedef struct {
	const char* key;
	size_t key_len;
	fp_t (*funcptr)(fp_t);
} key_mathfuncptr_pair;

typedef struct {
	char* key;
	fp_t value;
} key_constant_pair;

typedef struct {
	const char* key;
	fp_t (*funcptr)(const char*);
} key_strfuncptr_pair;

typedef struct {
	const char *key;
	void *(*func)(void*);	// the argument is always either a NULL or a wlist*
} key_funcptr_pair;


fp_t func_pass_get_result(const char*, size_t, int*);
fp_t constant_pass_get_result(const char*, size_t);

#endif
