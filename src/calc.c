#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/select.h>

#ifdef NO_GNU_READLINE
#include "rl_emul.h"
#else
#include <readline/readline.h>
#include <readline/history.h>
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

#define REQ_THRESHOLD 0.0000000001
#define BIG_THRESHOLD 1000000000.0

static int roughly_equal(fp_t a, fp_t b) {
	fp_t delta = a-b;
	return (fabs(delta) < REQ_THRESHOLD) ? 1 : 0;
}

static void report_result(fp_t r) {
	if (roughly_equal(r, FLOOR(r)) && r < BIG_THRESHOLD) {
		printf(resultfmti, r);
	} else { 
		printf(resultfmtd, r); 
	}
}

static void report_result_plain(fp_t r) {
	if (roughly_equal(r, FLOOR(r)) && r < BIG_THRESHOLD) {
		printf("%.0Lf\n", r);
	} else { 
		printf("%.12Lg\n", r); 
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

static void cleanup() {
	#ifdef NO_GNU_READLINE
	if (rl_emul_initialized()) 
		rl_emul_deinit();
	#endif
}


static void sig_handler(int signum) {
	cleanup();
	puts("\n");
	exit(signum);
}

int main(int argc, char* argv[]) {

	signal(SIGINT, sig_handler);
	set_precision(DEFAULT_PREC);
	
	char *pipe_buf;
	if (read_stdin_piped_input(&pipe_buf) > 0) {
		fp_t result;
		//fprintf(stderr, "%s\n", pipe_buf);
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
		fp_t result;
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
	
	printf("calc %s - using %s (%lu bit width) & %s.\n", 
	version_string, prec_config, fp_t_size_bits, rl_config);
	
#ifdef NO_GNU_READLINE
	rl_emul_init();
#endif
	char *input;

	struct udc_node *ans = udctree_add("ans", 0);

	while (quit_signal == 0) {

		sa_clearbuf();
		#ifdef NO_GNU_READLINE
		input = rl_emul_readline("");
		#else
		input = readline("");
		#endif
		if (!input) continue;	// to counter ^A^D (ctrl+A ctrl+D) segfault
		
		char *input_tidy = strip_duplicate_whitespace(strip_surrounding_whitespace(input));
		if (!input_tidy) { continue; }
		
		struct wlist_t wlist = wlist_generate(input_tidy, " ");
		int found = wlist_parse_command(&wlist);
		wlist_destroy(&wlist);

		if (found) { continue; }
		
		// no matching command was found, parse as mathematical input 
		// -> all whitespace can now be filtered, to simplify parsing
		// This validates invalid expressions such as " 5 + 5 3 51 5 3" though...

		fp_t result = 0;
		if (!parse_mathematical_input(input_tidy, &result)) {
			continue;	
		}

		report_result(result);
		
		#ifdef NO_GNU_READLINE
		rl_emul_hist_add(input);
		#else 
		add_history(input);
		#endif 

		ans->pair.value = result;
		free(input);
		input = NULL;

	}

	#ifdef NO_GNU_READLINE
	rl_emul_deinit();
	#endif

	return 0;

}
