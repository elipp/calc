#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "termtree.h"
#include "string_manip.h"
#include "string_allocator.h"
#include "tables.h"
#include "ud_constants_tree.h"
#include "functions.h"

static int errlevel = 0;

#define ERR_SYNTAX_MASK (0x1 << 6)
#define ERR_BAD_PARENTHESIS (ERR_SYNTAX_MASK | 0x1)
#define ERR_BAD_OPERATOR (ERR_SYNTAX_MASK | 0x2)
#define ERR_TRAILING_PARINPUT (ERR_SYNTAX_MASK | 0x4)
#define ERR_UNKNOWN_IDENTIFIER (ERR_SYNTAX_MASK | 0x8)
#define ERR_INTERNAL (0x1)

static struct term_t term_construct() {
	struct term_t t;

	fp_t_construct(&t.value);
	t.string = NULL;
	t.reparse = 0;

	return t;
}


static void term_destroy(struct term_t *t) {
	fp_t_destroy(&t->value);
}

static struct termtree_t termtree_create(int level);
static void termtree_add(struct termtree_t *t, const char* term_str, mathfuncptr op, int reparse);
static void termtree_resize(struct termtree_t *t);
static void termtree_destroy(struct termtree_t *t);
static int tokenize_term(const char* str, int level, struct termtree_t *tree);

static void term_get_result(struct term_t *term, int level);
static void term_convert_strtod(struct term_t *term);
static struct term_t term_create(const char* str, int reparse);
static void parse_error(int eno, const char* expr, int pos);


// misc helpers 
static int check_parenthesis_balance(const char* str);
static int find_matching_parenthesis(const char* str, int opening_par_pos);

static int check_parenthesis_balance(const char* str) {
	size_t str_len = strlen(str);

	int i = 0;
	int parlevel = 0;
	for (; i < str_len; ++i) {
		if (parlevel < 0) { parse_error(ERR_BAD_PARENTHESIS, str, i); return -1; }
		char c = str[i];
		if (c == '(') ++parlevel;		
		else if (c == ')') --parlevel;		
	}

	return parlevel;
}

static void parse_error(int eno, const char* expr, int pos) {
	if (eno & ERR_SYNTAX_MASK) {
		fprintf(stderr, "syntax error: ");
		switch(eno) {
			case ERR_BAD_PARENTHESIS:
				fputs("bad parenthesis balance and/or placement!\n", stderr);
				break;
			case ERR_BAD_OPERATOR:
				fputs("bad operator placement!\n", stderr);
				break;
			case ERR_TRAILING_PARINPUT:
				fprintf(stderr, "unexpected character input after closing \')\' at expr:%d\n", pos);
				break;
			case ERR_UNKNOWN_IDENTIFIER:
				fprintf(stderr, "unknown identifier \"%s\"\n", expr);
				break;
			default:
				break;
		}
	}
	else fputs("calc: internal parser error (this shouldn't be happening!)\n", stderr);

	errlevel = eno;
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

int parse_mathematical_input(const char* str, fp_t *val) {

	errlevel = 0;

	char *stripped = strip_all_whitespace(str);

	if (!check_alphanumeric_validity(stripped)) {
		sa_free(stripped);
		return 0;
	}

	int pb = check_parenthesis_balance(stripped);
	if (pb != 0) {
		parse_error(ERR_BAD_PARENTHESIS, "NULL", 0);
		sa_free(stripped);
		return 0;
	}

	struct term_t t = term_create(stripped, 1);

	term_get_result(&t, 0);
	fp_t_assign(val, t.value);

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
	struct term_t term = term_construct();
	term.string = sa_strdup(str);
	term.reparse = reparse;
	return term;
}

void termtree_destroy(struct termtree_t *t) {
	int i = 0;
	for (; i < t->num_terms; ++i) 
		term_destroy(&t->terms[i]);

	free(t->terms);
	free(t->ops);
}

static int term_has_strippable_parentheses(const char* str) {
	// this helper func is used to check whether or not the term string 
	// is enclosed by respective (same-level) parentheses.
	
	size_t str_len = strlen(str);

	if (!(str[0] == '(' && str[str_len-1] == ')')) return 0;

	int i = 1;
	int parlevel = 1;
	for (; i < str_len; ++i) {
		if (str[i] == '(') ++parlevel;
		else if (str[i] == ')') --parlevel;

		if (parlevel == 0) {
			return (i < str_len-1) ? 0 : 1;
		}
		
	}
	return 0;

}

int tokenize_term(const char* str, int level, struct termtree_t *tree) {

	struct level_specific_opfuncs {
		const char operators[2];
		mathfuncptr funcs[2];
	};

	static const struct level_specific_opfuncs level_ops[3] = 
	{ {{'+', '-'}, {f_add, f_sub}}, {{'*', '/'}, {f_mul, f_div}}, {{'^', '%'}, {f_pow, f_mod}} };

	if (!strlen(str)) return 0;
	*tree = termtree_create(level);

	struct level_specific_opfuncs of = level_ops[level];

	int term_beg_pos = 0;
	int i = 0;
	size_t term_str_len = strlen(str);

	#define OPERATOR(c) ((c) == of.operators[0] || (c) == of.operators[1])

	char last_char = str[term_str_len-1];
	if (OPERATOR(last_char)) {
		parse_error(ERR_BAD_OPERATOR, str, term_str_len-1);
		return -1;
	}

	for (; i < term_str_len; ++i) {
		char c = str[i];

		while (!(OPERATOR(c)) && i < term_str_len) {
			if (c == '(') {
				int par_end = find_matching_parenthesis(str, i);
				if (par_end < 0) { parse_error(ERR_BAD_PARENTHESIS, str, i); return -1; }
				else i = par_end;	// skip to par_end
			}
			++i;
			c = str[i];
		}
		
		int term_end_pos = i;
		int subterm_length = term_end_pos - term_beg_pos;

		if (subterm_length <= 0 && level > 0) {
			// an expression such as '5++--+-+--+-5' is considered to be syntactically valid,
			// while '5^*//****///5' isn't (hence the level>0 criterion)

			parse_error(ERR_BAD_OPERATOR, str, i);
			return -1;
		}

		// TODO: consider an early-out for L0 subterms with length 0.

		char *subterm = substring(str, term_beg_pos, subterm_length);

		mathfuncptr f = NULL;
		if (c == of.operators[0]) { f = of.funcs[0]; }
		else if (c == of.operators[1]) { f = of.funcs[1]; }

		int reparse = 0;

		if (term_has_strippable_parentheses(subterm)) {
			if (strlen(subterm) <= 2) { parse_error(ERR_BAD_PARENTHESIS, subterm, i); return -1; }
			char *subterm_stripped = strip_surrounding_parentheses(subterm);
			sa_free(subterm);
			subterm = subterm_stripped;
			reparse = 1;	// had parentheses -> needs full re-evaluation
		}

		termtree_add(tree, subterm, f, reparse);
		sa_free(subterm);

		term_beg_pos = i+1;
	
	}

	return tree->num_terms;

}

static int term_varfuncid_strcmp_pass(struct term_t *term) {

	int i = 0;
	for (; i < functions_table_size; ++i) {
		char *b;
		if ((b = strstr(term->string, functions[i].key)) == &term->string[0]) {
			int opening_par_pos = functions[i].key_len;

			int closing_par_pos = find_matching_parenthesis(term->string, opening_par_pos);
			int tlen = closing_par_pos - opening_par_pos;

			if (tlen <= 0) { 
				parse_error(ERR_INTERNAL, NULL, 0);
				return -1;
			} else if (closing_par_pos < (strlen(term->string) - 1)) {
				parse_error(ERR_TRAILING_PARINPUT, term->string, closing_par_pos + 1);
				return -1;
			}

			char *argstr = substring(term->string, opening_par_pos, tlen+1);
			struct term_t arg_term = term_create(argstr, 1);
			term_get_result(&arg_term, 0);
			
			functions[i].funcptr(&term->value, arg_term.value);
			sa_free(argstr);

			return 1;
		}
	}

	for (i = 0; i < constants_table_size; ++i) {
		if (strcmp(term->string, constants[i].key) == 0) {
			fp_t_assign(&term->value, constants[i].value);
			return 1;
		}
	}

	struct udc_node *n = udctree_search(term->string);
	if (n) {
		fp_t_assign(&term->value, n->pair.value);
		return 1;
	}

	return 0;
}

static void term_convert_strtod(struct term_t *term) {
	char *endptr = NULL;
	// do a function strcmp pass here
	int r = -1;
	if ((r = term_varfuncid_strcmp_pass(term)) <= 0) {
		// no matches->strtod.
		if (!to_fp_t(&term->value, term->string, &endptr))
			parse_error(ERR_UNKNOWN_IDENTIFIER, term->string, 0);
	}
}

static void term_get_result(struct term_t *term, int level) {
	
	if (errlevel > 0) {
		return;
	}

	struct termtree_t tree;

	int ret;
	if ((ret = tokenize_term(term->string, level, &tree)) < 0) {
//		fprintf(stderr, "WARNING: term tokenization failed!\n");
		termtree_destroy(&tree);
		errlevel = 1;
		return;
	} 
	
	else if (ret == 0) {
		// a return value of 0 signifies a NULL term string.
		return;
	}

	int i = 0;
	for (; i < tree.num_terms; ++i) {
		struct term_t *cur = &tree.terms[i];
		if (cur->reparse)
			term_get_result(cur, 0);
		
		else level >= 2 ? term_convert_strtod(cur) : term_get_result(cur, level+1);
		
	}

	fp_t result;
	fp_t_construct(&result);

	fp_t_assign(&result, tree.terms[0].value);

	for (i = 1; i < tree.num_terms; ++i) {
		tree.ops[i-1](&result, result, tree.terms[i].value);
	}

	fp_t_assign(&term->value, result);

	fp_t_destroy(&result);
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



