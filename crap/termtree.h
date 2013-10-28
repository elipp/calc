#ifndef TERMTREE_H
#define TERMTREE_H

#include "mathstuff.h"

struct term_t {
	char *string;
	double value;
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

int parse_mathematical_input(const char* str, double *val);

#endif
