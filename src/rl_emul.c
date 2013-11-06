#include "rl_emul.h"
#include "string_allocator.h"

static size_t rl_main_buffer_size = 1024;

/* ANSI escape sequences */

static const char* esc_cur_1_left = "\033[1D";
static const char* esc_cur_1_right = "\033[1C";
static const char* esc_cur_save = "\0337";
static const char* esc_cur_restore = "\0338";
static const char* esc_clear_cur_right = "\033[0K";
static const char* esc_composite_bkspc = "\033[1D \033[1D";
static const char* esc_cur_reset_left = "\r";
static const char* esc_composite_clear_line_reset_left = "\r\033[0K";
static const char* esc_composite_ml_bk_pre = "\033[1D \033[1D\0337\033[0K";

/* (static) function declarations */

static void gb_create_gap(size_t);
static void gb_merge();

static const char* hist_get_current(size_t*);

static int initialized = 0;

typedef struct _hist_node {
	struct _hist_node *next;
	struct _hist_node *prev;
	char* line_contents;
	size_t line_length;
} hist_node;

// the hist_ is essentially a doubly-linked list of strings.
static hist_node buffer_current = { NULL, NULL, NULL, 0 }; // this node is always on top of the list :P
static hist_node *hist_current = &buffer_current;
static hist_node *hist_head = &buffer_current;
static hist_node *hist_root = &buffer_current;

static size_t hist_size = 0;

/* main line-editing main_buffer */
static char *main_buffer = NULL;
static char ctrl_char_buf[8] = { 0 };

static void rl_emul_hist_destroy();

void rl_emul_hist_add(const char *arg) {
	
	if (!arg) { return; }

	const size_t arg_len = strlen(arg);
	hist_node *newnode = malloc(sizeof(hist_node));
	newnode->line_contents = strdup(arg);
	newnode->line_length = arg_len;

	if (hist_head == &buffer_current) {
		hist_head = newnode;
		hist_root = newnode;
		hist_head->next = NULL;
		hist_head->prev = NULL;
	}
	else {
		hist_head->next = newnode;	// this also sets hist_root->next = newnode, when hist_root == hist_head
		newnode->prev = hist_head;
		newnode->next = NULL;
		hist_head = newnode;
	}

	hist_head->next = &buffer_current;
	buffer_current.prev = hist_head;
	buffer_current.next = NULL;
	hist_current = &buffer_current;
	++hist_size;
}

static void rl_emul_hist_destroy() {
	// free node by node :P
	hist_node *iter = hist_root;
	while (iter != NULL && iter != &buffer_current) {
		hist_node *nexttmp = iter->next;
		free(iter->line_contents);
		free(iter);
		iter = nexttmp;
	}
}

static const char* hist_get_current(size_t *histlen_ret) {
	*histlen_ret = hist_current ? hist_current->line_length : 0;
	return hist_current->line_contents;
}

/*
static void hist_print_entries() {
	printf("\nHistory entries:\n");
	hist_node *iter = NULL;
	for (iter = hist_root; iter != NULL; iter = iter->next) {
		printf("%s\n", iter->line_contents);
	}

} */


static void rid_excess_chars_stdin(char ctrl) {
	if (ctrl >= 49 && ctrl <= 54) { 
		char k = getchar(); 
		if (k != 126) { getchar(); } 
	}
}

// anything prefixed with gb_ is related to the line-editing [g]ap [b]uffer

static enum { GB_NOEXIST = 0, GB_MIDLINE_INSERT, GB_MIDLINE_BACKSPACE } gb_exists;

static char *gb_post_buffer = NULL;
static size_t gb_post_length = 0;
static size_t gb_post_bufsize = 256;

static const size_t gap_width = 0xF;
	
static size_t cur_pos = 0; 
static size_t line_len = 0; // this represents the net length of the line (i.e., not including the potential gap in the main_buffer)

static struct termios old;


void rl_emul_init() {

	// to enable unbuffered stdin/out with getchar/putchar 
	struct termios new;
	tcgetattr(0, &old);
	new = old;
	new.c_lflag &=(~ICANON & ~ECHO);
	tcsetattr(0, TCSANOW, &new);

	main_buffer = malloc(rl_main_buffer_size);
	gb_post_buffer = malloc(gb_post_bufsize);

	initialized = 1;

}

void rl_emul_deinit() {
	// restore old termios attrs
	if (main_buffer) { free(main_buffer); main_buffer = NULL; }
	if (gb_post_buffer) { free(gb_post_buffer); gb_post_buffer = NULL; }
	tcsetattr(0, TCSANOW, &old);
	rl_emul_hist_destroy();
}

int rl_emul_initialized() { return initialized; }

static void gb_create_gap(size_t post_len) {
	
	if (gb_exists) gb_merge();

	if (post_len >= gb_post_bufsize-1) { 
		free(gb_post_buffer);
		gb_post_bufsize = ((post_len/256)+1)*256;
		gb_post_buffer = malloc(gb_post_bufsize);
	}

	gb_post_length = line_len - cur_pos;		
	memcpy(gb_post_buffer, main_buffer + cur_pos, gb_post_length);

	main_buffer[cur_pos] = '\0';
	gb_post_buffer[post_len] = '\0';
	gb_exists = GB_MIDLINE_INSERT;

}

static void gb_merge() {
	memcpy(main_buffer+cur_pos, gb_post_buffer, gb_post_length);
	main_buffer[line_len] = '\0';
	gb_post_length = 0;

	// gap no longer exists, reset flag
	gb_exists = GB_NOEXIST;
}


static void handle_ANSI_escape_seq() {
	ctrl_char_buf[0] = getchar();	
	ctrl_char_buf[1] = getchar();
	// sometimes, even more characters are printed, and these we need to get rid of; see rid_excess_chars_stdio()

	const char* hist_line = NULL;
	size_t hist_line_len = 0;
	
	if (ctrl_char_buf[0] == 91) {

		rid_excess_chars_stdin(ctrl_char_buf[1]);

		if (gb_exists) gb_merge();

		switch (ctrl_char_buf[1]) {
			case ARROW_LEFT:
				printf("%s", esc_cur_1_left);
				DECREMENT_CUR_POS_NZ();
				break;
			case ARROW_RIGHT:
				if (cur_pos < line_len) {
					printf("%s", esc_cur_1_right);
					++cur_pos; 
				}
				break;

			case ARROW_UP:
				// clear whole line and main_buffer
				printf("%s", esc_composite_clear_line_reset_left);
				// to enable real readline-like behavior, the current main_buffer
				// is stored at the first position.
				if (hist_current == &buffer_current) {
					if (gb_exists) gb_merge(); 
					if (buffer_current.line_contents) { free(buffer_current.line_contents); }
					buffer_current.line_contents = strndup(main_buffer, line_len);
					buffer_current.line_length = line_len;
				}

				HIST_CURRENT_DECREMENT();

				hist_line = hist_get_current(&hist_line_len);
				if (hist_line) {
					line_len = hist_line_len;
					cur_pos = line_len;
					memcpy(main_buffer, (const void*)hist_line, hist_line_len);
					main_buffer[hist_line_len] = '\0';	// just to be sure :P
				} 

				printf("%s", main_buffer);

				break;

			case ARROW_DOWN:
				HIST_CURRENT_INCREMENT();

				if (hist_current == &buffer_current) {	
					hist_line = buffer_current.line_contents;
					line_len = buffer_current.line_length;
					cur_pos = line_len;
					memcpy(main_buffer, (const void*)buffer_current.line_contents, buffer_current.line_length);
					main_buffer[buffer_current.line_length] = '\0';	
				}
				else {
					hist_line = hist_get_current(&hist_line_len);
					line_len = hist_line_len;
					cur_pos = line_len;
					memcpy(main_buffer, (const void*)hist_line, hist_line_len);
					main_buffer[hist_line_len] = '\0';	
				}

				printf("%s", esc_composite_clear_line_reset_left);
				printf("%s", main_buffer);

				break;
			case DELETE:

				if (cur_pos < line_len) {	// there can only be mid-line deletes :P
					const size_t rem_len = line_len - cur_pos;
					printf("%s%s%s%s", esc_cur_save, esc_clear_cur_right, main_buffer+cur_pos+1, esc_cur_restore);
					memmove(main_buffer + cur_pos, main_buffer + cur_pos + 1, rem_len);
					DECREMENT_LINE_LEN_NZ();
					main_buffer[line_len] = '\0';
				}

				break;
			default:
				break;
		}
	}
}

static void handle_regular_char_input(char c) {

	if (cur_pos >= rl_main_buffer_size-1) {
		rl_main_buffer_size *= 2;
		main_buffer = realloc(main_buffer, rl_main_buffer_size);
	
		if (!main_buffer) { fprintf(stderr, "rl_emul: buffer overflow->realloc error!\n"); exit(0); }
	}

	if (cur_pos < line_len && !gb_exists) {
		gb_create_gap(line_len - cur_pos);
		main_buffer[cur_pos] = c;
		INCREMENT_MARKERS();
		putchar(c);
		printf("%s%s%s", esc_cur_save, gb_post_buffer, esc_cur_restore); 
	}
	else { 
		// the whole point of the gap buffer arrangement is to avoid doing memmove()s on every single midline insert
		putchar(c); 
		
		if (gb_exists) {
			printf("%s%s%s", esc_cur_save, gb_post_buffer, esc_cur_restore);
		}

		main_buffer[cur_pos] = c;
		main_buffer[cur_pos+1] = '\0';
		INCREMENT_MARKERS();
	}
}


char *rl_emul_readline(const char* prompt) {

	memset(main_buffer, '\0', rl_main_buffer_size);	// probably a bit superfluous

	cur_pos = 0;
	line_len = 0;
	gb_post_length = 0;

	while (1) {
		char c;
		c = getchar();
		if (c == LINE_FEED) {	// enter, or line feed
			// if there's a gap in the buffer, merge
			if (gb_exists) { gb_merge(); }
			putchar(c);
			break;
		}
		else if (c == TAB_STOP) {
			// do nothing :P
			// the real readline library has some autocompletion features
		}
		else if (c == CTRL_A) {
			if (gb_exists) { gb_merge(); }
			printf("%s", esc_cur_reset_left);
			cur_pos = 0;
		}
		else if (c == CTRL_E) {
			if (gb_exists) { gb_merge(); }
			printf("%s", esc_cur_reset_left);
			printf("\033[%luC", line_len);
			cur_pos = line_len;
		}
		else if (c == CTRL_K) {
			if (gb_exists) { gb_merge(); }
			printf("%s", esc_clear_cur_right);
			main_buffer[cur_pos] = '\0';
		}
		else if (c == CTRL_E) {
			if (gb_exists) { gb_merge(); }
			printf("%s\033[%luC", esc_cur_reset_left, line_len);
			cur_pos = line_len;
		}
		else if (c == BACKSPACE) {	// backspace
			
			if (gb_exists == GB_MIDLINE_INSERT) {	
				gb_merge();	// this sets gb_exists = NOEXIST
			}
			
			int d = line_len - cur_pos;
			memmove(main_buffer+cur_pos-1, main_buffer+cur_pos, d);

			DECREMENT_CUR_POS_NZ();
			DECREMENT_LINE_LEN_NZ();
			main_buffer[line_len] = '\0';

			if (cur_pos < line_len) {	
				printf("%s%s%s", esc_composite_ml_bk_pre, main_buffer+cur_pos, esc_cur_restore);
			} else {
				printf("%s", esc_composite_bkspc);
			}

		}

		else if (c == '\033') {
			handle_ANSI_escape_seq(); // much cleaner this way, since the \033 block is rather complicated
		}
		else if (IS_REGULAR_INPUT(c)) {	// i.e. value between 0x20 and 0x7E. Mainly to avoid printing the unprintable :P
			handle_regular_char_input(c);
		}

	}
	
	if (line_len == 0) { return NULL; }
	else return strndup(main_buffer, line_len);

}



