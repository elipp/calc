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

struct termtree_t termtree_create(int level);
struct term_t term_create(const char* str, int reparse);
void termtree_destroy(struct termtree_t *t);
struct termtree_t tokenize_term(const char* str, int level);

int parse_mathematical_input(const char* str, double *val);

#endif
