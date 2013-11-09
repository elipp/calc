#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/select.h>

#include "rl_emul.h"
#include "definitions.h"
#include "termtree.h"
#include "string_manip.h"
#include "string_allocator.h"
#include "ud_constants_tree.h"
#include "tables.h"
#include "wlist.h"
#include "commands.h"

#ifdef USE_MPFR
static const char* prec_config = "the GNU MPFR library";

#elif LONG_DOUBLE_PRECISION
static const char* prec_config = "long double as fp_t";

#else
static const char* prec_config = "double as fp_t";
#endif

static const char* rl_config = "bundled rl_emul";
static const char* version_string = "0.1.1a";

extern int quit_signal;

static int read_stdin_piped_input(char** buffer) {

	if (isatty(STDIN_FILENO)) {
		// thank god for isatty :D
		return 0;
	}
	
	#define PIPEBUF_CHUNKSIZE (8*1024)

	size_t bufsize = PIPEBUF_CHUNKSIZE;
	char* b = malloc(bufsize);
	char *offset = b;
	int r = -1;
	size_t total_bytes = 0;

	while ((r = fread(offset, 1, PIPEBUF_CHUNKSIZE, stdin)) > 0) {
		total_bytes += r;
		size_t prev_bufsize = bufsize;
		bufsize += PIPEBUF_CHUNKSIZE;
		b = realloc(b, bufsize);
		if (!b) { fprintf(stderr, "realloc error :(\n"); return 0; }
		offset = b + prev_bufsize;
	}

	if (total_bytes < 1) { free(b); return total_bytes; }
	*buffer = b;

	return 1;

}

#define BIG_THRESHOLD 1000000000.0

static void report_result(fp_t r) {
	fputs("\033[1;29m= ", stdout);
	fp_t_print(r, precision);
	fputs("\033[m\n", stdout);
}

static void report_result_plain(fp_t r) {
	fp_t_print(r, precision);
	fputc('\n', stdout);
}

static char *concatenate_argv(int argc, char *argv[]) {
	size_t tlen = 0;
	int i = 1;
	for (; i < argc; ++i) 
		tlen += strlen(argv[i]);

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

static void cleanup() {
	if (rl_emul_initialized()) 
		rl_emul_deinit();
	udctree_destroy();
	clean_constants();
}


static void sig_handler(int signum) {
	cleanup();
	exit(signum);
}

int main(int argc, char* argv[]) {

	signal(SIGINT, sig_handler);
	set_precision(DEFAULT_PREC);
	
	setup_constants();
	
	fp_t result;
	fp_t_construct(&result);

	char *pipe_buf;
	if (read_stdin_piped_input(&pipe_buf) > 0) {
		if (!parse_mathematical_input(pipe_buf, &result)) {
			fprintf(stderr, "error: parse_mathematical_input failed!\n");
			free(pipe_buf);
			return 1;
		}
		else {
			report_result_plain(result);
			free(pipe_buf);
			return 0;
		}
	}

	else if (argc > 1) {
		// we presumably are being presented with a mathexpr as command-line argument
		char *argv_collected = concatenate_argv(argc, argv);		
		if (!parse_mathematical_input(argv_collected, &result)) {
			fprintf(stderr, "error: parse_mathematical_input failed!\n");
			free(argv_collected);
			return 1;
		}
		else {
			report_result_plain(result);
			free(argv_collected);
			return 0;
		}
	}
	
	printf("calc %s - using %s & %s.\n", 
	version_string, prec_config, rl_config);
	
	rl_emul_init();
	char *input;

	struct udc_node *ans = udctree_add("ans", result);

	while (quit_signal == 0) {

		sa_clearbuf();
		input = rl_emul_readline("");
		
		if (!input) continue;	// to counter ^A^D (ctrl+A ctrl+D) segfault
		
		char *input_tidy = strip_duplicate_whitespace(strip_surrounding_whitespace(input));
		if (!input_tidy) { continue; }
			
		rl_emul_hist_add(input);
		
		struct wlist_t wlist = wlist_generate(input_tidy, " ");
		int found = wlist_parse_command(&wlist);
		wlist_destroy(&wlist);

		if (found) { continue; }
		
		// no matching command was found, parse as mathematical input 
		// -> all whitespace can now be filtered, to simplify parsing
		// This validates invalid expressions such as " 5 + 5 3 51 5 3" though...

		if (!parse_mathematical_input(input_tidy, &result)) {
			continue;	
		}

		report_result(result);
		fp_t_assign(&ans->pair.value, result);

		free(input);
		input = NULL;

	}

	fp_t_destroy(&result);

	rl_emul_deinit();

	return 0;

}
