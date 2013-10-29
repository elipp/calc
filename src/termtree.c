#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "termtree.h"
#include "string_manip.h"
#include "string_allocator.h"
#include "tables.h"

static int errlevel = 0;

static struct termtree_t termtree_create(int level);
static void termtree_add(struct termtree_t *t, const char* term_str, mathfuncptr op, int reparse);
static void termtree_resize(struct termtree_t *t);
static void termtree_destroy(struct termtree_t *t);
static int tokenize_term(const char* str, int level, struct termtree_t *tree);

static void term_get_result(struct term_t *term, int level);
static void term_convert_strtod(struct term_t *term);
static struct term_t term_create(const char* str, int reparse);

double_t to_double_t(const char* arg, char **endptr) { 
#ifdef C99_AVAILABLE 
	double_t res = strtold(arg, endptr);	// requires C99
#else
	double_t res = strtod(arg, endptr);
#endif
	return res;
}

// misc helpers 
static int check_parenthesis_balance(const char* str);
static int find_matching_parenthesis(const char* str, int opening_par_pos);

static int check_parenthesis_balance(const char* str) {
	size_t str_len = strlen(str);

	int i = 0;
	int parlevel = 0;
	for (; i < str_len; ++i) {
		if (parlevel < 0) { fprintf(stderr, "syntax error: parlevel < 0 at expr:%d\n", i); errlevel = 1; return -1; }
		char c = str[i];
		if (c == '(') ++parlevel;		
		else if (c == ')') --parlevel;		
	}

	return parlevel;
}


static int find_matching_parenthesis(const char* str, int opening_par_pos) {

	size_t str_len = strlen(str);
	int parlevel = 1;

	int i = opening_par_pos+1;
	for(; i < str_len; ++i) {
		char c = str[i];
		if (c == '(') ++parlevel;		
		else if (c == ')') {
			--parlevel;		
			if (parlevel == 0) {
				return i;
			}
		}
	}

	return -1;
	
}

int parse_mathematical_input(const char* str, double *val) {

	errlevel = 0;

	char *stripped = strip_all_whitespace(str);
	if (!check_alphanumeric_validity(stripped)) {
		sa_free(stripped);
		return 0;
	}

	int pb = check_parenthesis_balance(stripped);
	if (pb != 0) {
		fprintf(stderr, "error, unmatched parentheses (parlevel = %d)\n", pb);
		sa_free(stripped);
		return 0;
	}

	struct term_t t = term_create(str, 1);

	term_get_result(&t, 0);
	*val = t.value;

	sa_free(stripped);

	return errlevel == 0;

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
	term.string = sa_strdup(str);
	term.value = .0f;
	term.reparse = reparse;
	return term;
}

void termtree_destroy(struct termtree_t *t) {
	int i = 0;
	for (; i < t->num_terms; ++i) 
		sa_free(t->terms[i].string);

	free(t->terms);
	free(t->ops);
}

int tokenize_term(const char* str, int level, struct termtree_t *tree) {

	struct level_specific_opfuncs {
		const char operators[2];
		mathfuncptr funcs[2];
	};

	static const struct level_specific_opfuncs level_ops[3] = 
	{ {{'+', '-'}, {f_add, f_sub}}, {{'*', '/'}, {f_mul, f_div}}, {{'^', '%'}, {f_pow, f_mod}} };


	*tree = termtree_create(level);
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
				if (par_end < 0) { fprintf(stderr, "error: unmatched parenthesis at %d\n", i); return 0; }
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
			char *subterm_stripped = strip_surrounding_parentheses(subterm);
			sa_free(subterm);
			subterm = subterm_stripped;
			++term_beg_pos;
			--term_end_pos;
			reparse = 1;	// had parentheses -> needs full re-evaluation
		}

		termtree_add(tree, subterm, f, reparse);
		sa_free(subterm);

		term_beg_pos = i+1;
	
	}

	return 1;

}

static int term_varfuncid_strcmp_pass(struct term_t *term) {
	typedef double (*unary_mathfuncptr)(double);
	struct func_str_pair {
		const char *name;
		unary_mathfuncptr fptr;
	};

	int i = 0;
	for (; i < functions_table_size; ++i) {
		char *b;
		if ((b = strstr(term->string, functions[i].key)) == &term->string[0]) {
			int opening_par_pos = functions[i].key_len;
			if (term->string[functions[i].key_len] != '(') {
				fprintf(stderr, "funcpass: term->string[functions[i].key_len] != \'(\') (is %c)\n", term->string[functions[i].key_len]);
				errlevel = 1;
				return 0;
			}
			int closing_par_pos = find_matching_parenthesis(term->string, opening_par_pos);
			int tlen = closing_par_pos - opening_par_pos;

			if (tlen <= 0) { 
				fprintf(stderr, "parse error: failure extracting function argstr!\n");
				errlevel = 1;
				return 0;
			} else if (closing_par_pos != (strlen(term->string) - 1)) {
				fprintf(stderr, "syntax error: unexpected char input after argstr closing parenthesis!\n");
				errlevel = 1;
				return 0;
			}

			char *argstr = substring(term->string, opening_par_pos, tlen+1);
			fprintf(stderr, "argstr = \"%s\"\n", argstr);
			struct term_t arg_term = term_create(argstr, 1);
			term_get_result(&arg_term, 0);
			
			term->value = functions[i].funcptr(arg_term.value);
			sa_free(argstr);

			return 1;
		}
	}

	return 0;
}

static void term_convert_strtod(struct term_t *term) {
	char *endptr = NULL;
	// do a funtction strcmp pass here
	if (!term_varfuncid_strcmp_pass(term)) {
		// no matches->strtod.
		double_t val = to_double_t(term->string, &endptr);
		if (endptr != term->string + strlen(term->string)) {
			fprintf(stderr, "syntax error: strtod(\"%s\"): only %d first char(s) used in str->double conversion!\n", term->string, (int)(endptr - term->string));
			errlevel = 1;
			term->value = 0;
		}

		else {
			term->value = val;
		}
	}
}

static void term_get_result(struct term_t *term, int level) {
	
	if (errlevel > 0) {
		return;
	}

	struct termtree_t tree;

	if (!tokenize_term(term->string, level, &tree)) {
		fprintf(stderr, "WARNING: term tokenization failed!\n");
		term->value = 0;
		termtree_destroy(&tree);
		//errlevel = 1;
		return;
	}

	else if (tree.num_terms == 0) {
		fprintf(stderr, "WARNING: tree.num_terms == 0!\n");
		term->value = 0;
		termtree_destroy(&tree);
		// errlevel = 1;
		return;
	}

	int i = 0;
	for (; i < tree.num_terms; ++i) {
		struct term_t *cur = &tree.terms[i];
		if (cur->reparse)
			term_get_result(cur, 0);
		
		else level >= 2 ? term_convert_strtod(cur) : term_get_result(cur, level+1);
		
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



