#include "commands.h"
#include "functions.h"
#include "wlist.h"
#include "string_manip.h"
#include "string_allocator.h"
#include "termtree.h"

#define PRECSTRING_MAX 128

#ifdef LONG_DOUBLE_PRECISION
const char *resultfmti = "= \033[1;29m%1.0Lf\033[m\n";
#else
const char *resultfmti = "= \033[1;29m%1.0f\033[m\n";
#endif

char resultfmtd[PRECSTRING_MAX] = { 0 };	
int precision = DEFAULT_PREC;

extern const key_mathfuncptr_pair functions[];
extern const size_t functions_table_size;

extern int clashes_with_predefined(const char*);	// from tables.c

int quit_signal = 0;

void my(struct wlist_t *wlist) {

	// add variable to user-defined constant list (udc_list)
	// check that the my-invocation was properly formatted

	if (wlist->num_words <= 1) {
		fputs("my: missing argument!\n", stderr);
		return; 
	}

	if (strcmp(wlist->strings[1], "list") == 0) {
		my_list();			
		return;
	}

	char *recomposed = wlist_recompose(wlist);

	struct wlist_t retokenized = wlist_generate(recomposed, "=");

	if (retokenized.num_words != 2) { 
		printf("my: error: exactly 1 \'=\' expected (got %d). See \"help my\".\n", retokenized.num_words); 
		wlist_destroy(&retokenized);
		sa_free(recomposed);
		return;
	}

	// this is admittedly crappy, but at least it reuses existing code :D
	// the purpose of this is to tidy up the input string into a more suitable format.
	// "  my    x=            5    * 5    " -> "my x = 5*5"

	char *rerecomposed = wlist_recompose(&retokenized);
	struct wlist_t wlist_tidy  = wlist_generate(rerecomposed, " ");
	
	char* varname = sa_strdup(wlist_tidy.strings[1]);	
	char* valstring = sa_strdup(retokenized.strings[1]);	// token after the '=' char

	wlist_destroy(&retokenized);
	wlist_destroy(&wlist_tidy);

	if (DIGIT(varname[0])) {
		fputs("\nmy: error: variable identifiers cannot begin with digits.\n", stderr);
	}
	else if (clashes_with_predefined(varname)) {
		// the call prints its own error message
	}
	else {
		// check if already exists in the udctree.
		struct udc_node *search_result = udctree_search(varname);

		fp_t value;
		fp_t_construct(&value);
	
		parse_mathematical_input(valstring, &value);

		if (search_result) 
			fp_t_assign(&search_result->pair.value, value);
		else 
			udctree_add(varname, value); 

		fp_t_destroy(&value);
	}

	sa_free(recomposed);
}


void my_list(struct wlist_t *list) {
	int i = 0;
	struct udc_node *iter = udctree_get_root();
	if (iter) {
		printf("\nUser-defined constants:\n\nkey\tvalue\n");
		while (iter) {
			printf(" %s\t", iter->pair.key);
			fp_t_print(iter->pair.value, 8);
			putchar('\n');
			iter = iter->next;
			++i;
		}
		printf("\nTotal: %d\n", i);
	}
	else {
		printf("\nNo user-defined constants found. See \"help my\" for additional information.\n\n");
	}
}

void help(struct wlist_t *wlist) {

	if (wlist->num_words == 1) {
	printf(
"\ncalc is a calculator application, meaning it parses input\n\
mathematical expressions and computes the corresponding answer.\n\
\n\
GNU readline-style line-editing and input history is supported.\n\
\n\
Try \"help functions\" for a list of supported functions,\n\
    \"help constants\" for a list of built-in constants,\n\
    \"help my\" for information on the \"my\" command.\n\
    \"help set\" for information on the \"set\" command.\n\
\n");
	} else {
		if (wlist->num_words == 2) {
			const char *keyword = wlist->strings[1];
			// the list of help indices is small enough to just do if strcmp...else if
			// TODO: no it isn't
			if (strcmp(keyword, "functions") == 0) {
				help_functions();
			}
			else if (strcmp(keyword, "constants") == 0) {
				help_constants();
			}
			else if (strcmp(keyword, "my") == 0) {
				help_my();
			}
			else if (strcmp(keyword, "set") == 0) {
				help_set();
			}
			else { 
				printf("\nhelp: unknown help index \"%s\".\n", keyword);
			}
		}
		else { printf("help: trailing character(s) (\"%s...\")\n", wlist->strings[2]); }
	}

}

void help_functions(struct wlist_t *wlist) {

	static const size_t num_col = 6;
	// extern size_t functions_table_size
	printf("\nSupported mathematical functions:\n");
	size_t i = 0;
	size_t j = 0;
	while (j < functions_table_size/num_col) {
		i = 0;
		while (i < num_col) {
			printf("%s \t", functions[num_col*j+i].key);
			++i;
		}
		printf("\n");
		++j;
	}
	size_t d = functions_table_size - num_col*j;
	i = 0;
	while (i < d) {
		printf("%s \t", functions[num_col*j+i].key);
		++i;
	}
	printf("\n");

}

void help_constants(struct wlist_t *wlist) {

	// extern size_t constants_table_size
	printf("\nBuilt-in constants (scientific constants are in SI units):\n\nkey\tvalue\n");
	size_t i = 0;
	while (i < constants_table_size) {
		printf(" %s \t", constants[i].key);
		fp_t_print(constants[i].value, 8);
		putchar('\n');
		++i;
	}
	printf("\nUser-defined constants (variables) can be added with the command \"my <var-name> = <value>\".\n\n");

}

void help_my(struct wlist_t *wlist) {
	printf("\n\
The \"my\" command can be used to add user-defined constants to the program, i.e. to associate\n\
a string literal, either a single character or an arbitrarily long string, with a particular\n\
(decimal) value. However, some naming restrictions apply: pre-defined (builtin) constants or\n\
function names can not be overridden (see \"help constants/functions\" for a list of reserved keywords).\n\
Also, the identifier shouldn't begin with a digit.\n\
\n\
The lifetime of a variable added with \"my\" is until the session's termination.\n\
\n\
Usage: my <var-name> = <value>.\n\
\n\
Example:\n\
\n\
my x = sin((3/4)pi)\n\
\n");
}

void help_set() {
	printf("\n\
The \"set\" command can be used to control calc's inner workings. Available subcommands are:\n\
	set precision <integer argument>, controls how many decimals are shown in the final result\n\
\n");
}

void set(struct wlist_t *wlist) {
	if (wlist->num_words < 2) {
		printf("\nset: error: expected subcommand, none supplied\n");
		return;
	}

	char *subcommand = wlist->strings[1];
	if (strcmp(subcommand, "precision") == 0) {
		if (wlist->num_words < 3) { 
			printf("\nset precision: error: numerical argument expected.\n");
			return;
		}
		char* precstring = wlist->strings[2];

		fp_t prec_val;
		fp_t_construct(&prec_val);

		if (!parse_mathematical_input(precstring, &prec_val)) {
			fprintf(stderr, "set precision: bad arg\n");
		}
		else {
			set_precision(fp_t_to_long(prec_val));
		}
		fp_t_destroy(&prec_val);
	}

}

void set_precision(long prec) {

	if (prec < 1) { printf("set precision: error: precision requested < 1\n"); return; }
	#ifdef USE_MPFR
	mpfr_set_default_prec(prec);
	#elif LONG_DOUBLE_PRECISION
	if (prec > DEFAULT_PREC) { 
		printf("set precision: \033[1;31mwarning: incorrect decimals will almost certainly be included (p > %d)\033[m\n", DEFAULT_PREC); 
		if (prec > 56) { prec = 50; printf("Clamped precision to 50 decimal places.\n"); }
	}
	precision = prec;

	#endif
}

void quit() {
	quit_signal = 1;
}

const key_funcptr_pair commands[] = {
{ "help", help },
{ "my", my },
{ "quit", quit },
{ "set", set }
};


const size_t commands_table_size = sizeof(commands)/sizeof(commands[0]); 

