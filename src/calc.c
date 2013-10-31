#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/select.h>

#ifdef NO_GNU_READLINE
#include "rl_emul.h"
#else
#include <readline/readline.h>
#endif

#include "definitions.h"
#include "termtree.h"
#include "string_manip.h"
#include "string_allocator.h"
#include "ud_constants_tree.h"
#include "tables.h"
#include "wlist.h"
#include "commands.h"

extern char resultfmtd[];
extern const char *resultfmti;

#ifdef LONG_DOUBLE_PRECISION
static const char* prec_config = "long double as fp_t";
#else
static const char* prec_config = "double as fp_t";
#endif

#ifdef NO_GNU_READLINE
static const char* rl_config = "bundled rl_emul";
#else 
static const char* rl_config = "GNU readline";
#endif

static const char* version_string = "0.1a";

static const size_t fp_t_size_bits = sizeof(fp_t) * 8;

extern int quit_signal;

static int read_stdin_piped_input(char** buffer) {

	fd_set fds;
	struct timeval tv;
	FD_ZERO(&fds);
	FD_SET(STDIN_FILENO, &fds);
	tv.tv_sec = 0;
	tv.tv_usec = 0;

	int ret = select(STDIN_FILENO+1, &fds, NULL, NULL, &tv);
	if (ret < 0) { fprintf(stderr, "read_stdin_piped_input: select() error\n"); return 0; }

	if (FD_ISSET(STDIN_FILENO, &fds)) {
		#define PIPEBUFSZ 512
		*buffer = malloc(PIPEBUFSZ);
		char *bmirror = *buffer;
		char *r = fgets(bmirror, PIPEBUFSZ, stdin);
		if (r == NULL) { fprintf(stderr, "warning: fgets returned NULL!\n"); }
		size_t input_len = strlen(bmirror);
		bmirror[input_len-1] = '\0';
		return (int)input_len;
	}
	else {
		return 0;
	}


}

static int roughly_equal(fp_t a, fp_t b) {
	fp_t delta = a-b;
	return (fabs(delta) < 0.00000000000000001) ? 1 : 0;
}

static void report_result(fp_t r) {
	if (roughly_equal(r, FLOOR(r))) {
		printf(resultfmti, r);
	} else { 
		printf(resultfmtd, r); 
	}
}

static char *concatenate_argv(int argc, char *argv[]) {
	size_t tlen = 0;
	int i = 1;
	for (; i < argc; ++i) {
		tlen += strlen(argv[i]);
	}

	size_t bufsize = tlen+argc+1;
	char *buffer = malloc(bufsize);
	memset(buffer, '\0', bufsize);
	
	for (i = 1; i < argc; ++i) {
		strcat(buffer, argv[i]);
		strcat(buffer, " ");
	}
	buffer[bufsize-1] = '\0';

	return buffer;
}

int main(int argc, char* argv[]) {

	printf("calc %s - using %s (%lu bit width) & %s.\n", 
	version_string, prec_config, fp_t_size_bits, rl_config);

	set_precision(DEFAULT_PREC);

	char *pipe_buf;
	if (read_stdin_piped_input(&pipe_buf) > 0) {
		fprintf(stderr, "reading input from stdin (not entering interactive mode).\n");
		fp_t result;
		if (!parse_mathematical_input(pipe_buf, &result)) {
			fprintf(stderr, "error: parse_mathematical_input failed!\n");
			free(pipe_buf);
			return 1;
		}
		else {
			report_result(result);
			free(pipe_buf);
			return 0;
		}
	}

	else if (argc > 1) {
		// we presumably are being presented with a mathexpr as command-line argument
		fprintf(stderr, "reading input from argv (not entering interactive mode).\n");
		char *argv_collected = concatenate_argv(argc, argv);		
		fp_t result;
		if (!parse_mathematical_input(argv_collected, &result)) {
			fprintf(stderr, "error: parse_mathematical_input failed!\n");
			free(argv_collected);
			return 1;
		}
		else {
			report_result(result);
			free(argv_collected);
			return 0;
		}
	}
	

#ifdef NO_GNU_READLINE
	rl_emul_init();
#endif
	char *input;

	udc_node *ans = udc_node_create("ans", 0);
	udctree_add(ans);

	while (quit_signal == 0) {
		#ifdef NO_GNU_READLINE
		input = rl_emul_readline("");
		#else
		input = readline("");
		#endif
		if (!input) continue;	// to counter ^A^D (ctrl+A ctrl+D) segfault
		char *input_stripped = strip_surrounding_whitespace(input);

		if (input_stripped) { 
			struct wlist_t wlist = wlist_generate(input_stripped, " ");
			int found = wlist_parse_command(&wlist);
			if (!found) {
				// no matching command was found, parse as mathematical input 
				// -> all whitespace can now be filtered, to simplify parsing
				// This validates invalid expressions such as " 5 + 5 3 51 5 3" though...

				if (!input_stripped) { goto cont; }	// strip_all_whitespace returns NULL if input is completely whitespace
				fp_t result = 0;
				if (!parse_mathematical_input(input_stripped, &result)) {
					goto cont;
				}

				report_result(result);
			
				#ifdef NO_GNU_READLINE
				rl_emul_hist_add(input_stripped);
				#else 
				add_history(input_stripped);
				#endif 
				ans->pair.value = result;

			}
		cont:
			wlist_destroy(&wlist);
						
		}

		sa_free(input_stripped);
		sa_clearbuf();
	}

	#ifdef NO_GNU_READLINE
	rl_emul_deinit();
	#endif

	return 0;

}
