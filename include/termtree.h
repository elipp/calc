#ifndef TERMTREE_H
#define TERMTREE_H

#include "mathstuff.h"
#include "definitions.h"

struct term_t {
	char *string;
	double_t value;
	int reparse;
};


struct termtree_t {
	struct term_t *terms;
	mathfuncptr *ops;
	int level;
	int num_terms;
	int capacity;
	int id;	// debug
};

double_t to_double_t(const char* arg, char **endptr);
int parse_mathematical_input(const char* str, double_t *val);

#endif
