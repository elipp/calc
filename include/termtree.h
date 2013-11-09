#ifndef TERMTREE_H
#define TERMTREE_H

#include "functions.h"
#include "definitions.h"

struct term_t {
	char *string;
	fp_t value;
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

void to_double_t(fp_t *t, const char* arg, char **endptr);
int parse_mathematical_input(const char* str, fp_t *val);

#endif
