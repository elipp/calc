#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "termtree.h"
#include "string_manip.h"

static void termtree_add(struct termtree_t *t, const char* term_str, mathfuncptr op, int reparse);
static void termtree_resize(struct termtree_t *t);
static double termtree_gather_result(struct termtree_t *tree);

static void term_reparse(struct term_t *term);
static void term_get_result(struct term_t *term, int level);
static void term_convert_strtod(struct term_t *term);

// misc utils
static int check_brace_balance(const char* str);
static int find_matching_parenthesis(const char* str, int par_beg_index);

static int check_brace_balance(const char* str) {
	size_t str_len = strlen(str);

	int i = 0;
	int brace_level = 0;
	for (; i < str_len; ++i) {
		if (brace_level < 0) { fprintf(stderr, "syntax error: brace_balance < 0 at expr:%d\n", i); return -1; }
		char c = str[i];
		if (c == '(') ++brace_level;		
		else if (c == ')') --brace_level;		
	}

	return brace_level;
}


static int find_matching_parenthesis(const char* str, int par_beg_index) {

	size_t str_len = strlen(str);
	int brace_level = 1;

	int i = par_beg_index+1;
	for(; i < str_len; ++i) {
		char c = str[i];
		if (c == '(') ++brace_level;		
		else if (c == ')') {
			--brace_level;		
			if (brace_level == 0) {
				return i;
			}
		}
	}

	return -1;
	
}

int parse_mathematical_input(const char* str, double *val) {

	char *stripped = strip_all_whitespace(str);

	int bb = check_brace_balance(stripped);
	if (bb != 0) {
		fprintf(stderr, "error, unmatched parentheses (brace_level = %d)\n", bb);
		free(stripped);
		return 0;
	}

	struct termtree_t t = tokenize_term(stripped, 0);

	*val = termtree_gather_result(&t);

	free(stripped);
	termtree_destroy(&t);

	return 1;

}


struct termtree_t termtree_create(int level) {
	
	static int termtree_id = 0;

	struct termtree_t t;
	t.num_terms = 0;
	t.level = level;
	t.capacity = 16;
	t.terms = malloc(t.capacity*sizeof(struct term_t));
	t.ops = malloc(t.capacity*sizeof(mathfuncptr));
	t.id = termtree_id++;

	return t;
}

struct term_t term_create(const char* str, int reparse) {
	struct term_t term;
	term.string = strdup(str);
	term.value = .0f;
	term.reparse = reparse;
	return term;
}

void termtree_destroy(struct termtree_t *t) {
	int i = 0;
	for (; i < t->num_terms; ++i) {
		free(t->terms[i].string);
	}
	free(t->terms);
	free(t->ops);
}

struct termtree_t tokenize_term(const char* str, int level) {

	struct level_specific_opfuncs {
		const char operators[2];
		mathfuncptr funcs[2];
	};

	static const struct level_specific_opfuncs level_ops[3] = 
	{ {{'+', '-'}, {f_add, f_sub}}, {{'*', '/'}, {f_mul, f_div}}, {{'^', '%'}, {f_pow, f_mod}} };


	struct termtree_t tree = termtree_create(level);
	struct level_specific_opfuncs of = level_ops[level];

	int term_beg_pos = 0;
	int i = 0;
	size_t term_str_len = strlen(str);
	for (; i < term_str_len; ++i) {
		char c = str[i];

		while (c != of.operators[0]
		    && c != of.operators[1]
		    && i < term_str_len) {
			if (c == '(') {
				int par_end = find_matching_parenthesis(str, i);
				if (par_end < 0) { fprintf(stderr, "error: unmatched parenthesis at %d\n", i); return tree; }
				else i = par_end;	// skip to par_end
			}
			++i;
			c = str[i];
		}

		int term_end_pos = i;

		int subterm_length = term_end_pos - term_beg_pos;
		char *subterm = substring(str, term_beg_pos, subterm_length);

		mathfuncptr f = NULL;
		if (c == of.operators[0]) { f = of.funcs[0]; }
		else if (c == of.operators[1]) { f = of.funcs[1]; }

		int reparse = 0;

		while (str[term_beg_pos] == '(' && str[term_end_pos-1] == ')') {
			char *subterm_stripped = strip_outer_braces(subterm);
			free(subterm);
			subterm = subterm_stripped;
			++term_beg_pos;
			--term_end_pos;
			reparse = 1;	// had braces -> needs full re-evaluation
		}

		termtree_add(&tree, subterm, f, reparse);
		free(subterm);

		term_beg_pos = i+1;
	
	}

	return tree;

}

static void term_reparse(struct term_t *term) {
	struct termtree_t rptree = tokenize_term(term->string, 0);
	term->value = termtree_gather_result(&rptree);
	termtree_destroy(&rptree);
}

static void term_convert_strtod(struct term_t *term) {
	char *dummy = NULL;
	term->value = strtod(term->string, &dummy);
}

static double termtree_gather_result(struct termtree_t *tree) {

	// assuming termtree_level = 0, ie. +/- 

	int i = 0;
	for (; i < tree->num_terms; ++i) {
		if (tree->terms[i].reparse) {
			term_reparse(&tree->terms[i]);
		}
		else {
			term_get_result(&tree->terms[i], 1);
		}
	}


	double total = tree->terms[0].value;
	for (i = 1; i < tree->num_terms; ++i) {
		total = tree->ops[i-1](total, tree->terms[i].value);
	}
	
	return total;
}


static void term_get_result(struct term_t *term, int level) {
	struct termtree_t tree = tokenize_term(term->string, level);

	if (tree.num_terms == 0) {
		fprintf(stderr, "WARNING: tree.num_terms == 0!\n");
		term->value = 0;
		return;
	}

	int i = 0;
	for (; i < tree.num_terms; ++i) {
		if (tree.terms[i].reparse) {
			term_reparse(&tree.terms[i]);
		} else {
			if (level >= 2) {
				term_convert_strtod(&tree.terms[i]);
			} else {
				// fcking finally! proper recursion :)
				term_get_result(&tree.terms[i], level+1);
			}
		}
	}

	double result = 0.0;
	result = tree.terms[0].value;

	for (i = 1; i < tree.num_terms; ++i) {
		result = tree.ops[i-1](result, tree.terms[i].value);
	}

	term->value = result;

	termtree_destroy(&tree);

}



static void termtree_add(struct termtree_t *t, const char* term_str, mathfuncptr op, int reparse) {
	if (t->num_terms >= t->capacity) {
		termtree_resize(t);
	}
	t->terms[t->num_terms] = term_create(term_str, reparse);
	t->ops[t->num_terms] = op;
	++t->num_terms;
}

static void termtree_resize(struct termtree_t *t) {
	t->capacity *= 2;
	t->terms = realloc(t->terms, t->capacity*sizeof(struct term_t));
	t->ops = realloc(t->ops, t->capacity*sizeof(mathfuncptr));
}



