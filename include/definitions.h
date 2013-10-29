#ifndef DEFINITIONS_H
#define DEFINITIONS_H 

#include <stdlib.h>

#ifdef LONG_DOUBLE_PRECISION
#define DEFAULT_PREC 16
typedef long double double_t;
#else
#define DEFAULT_PREC 10
typedef double double_t;
#endif 

typedef struct {
	const char* key;
	size_t key_len;
	double_t (*funcptr)(double_t);
} key_mathfuncptr_pair;

typedef struct {
	char* key;
	double_t value;
} key_constant_pair;

typedef struct {
	const char* key;
	double_t (*funcptr)(const char*);
} key_strfuncptr_pair;

typedef struct {
	const char *key;
	void *(*func)(void*);	// the argument is always either a NULL or a wlist*
} key_funcptr_pair;


double_t func_pass_get_result(const char*, size_t, int*);
double_t constant_pass_get_result(const char*, size_t);

#endif
