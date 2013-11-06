#ifndef RL_EMUL_H
#define RL_EMUL_H

#include <string.h>
#include <termios.h>
#include <stdlib.h>
#include <stdio.h>

#define IS_REGULAR_INPUT(x) ((x) > 0x19 && (x) < 0x7F)

#define DECREMENT_CUR_POS_NZ()\
	do {\
		if (cur_pos > 0) { --cur_pos; }\
	} while(0)

#define DECREMENT_LINE_LEN_NZ() \
	do {\
		if (line_len > 0) { --line_len; }\
	} while(0)

#define INCREMENT_MARKERS()\
	do {\
		++cur_pos;\
		++line_len;\
	} while(0)

/* the following macros are debugging utility macros. */

#define BUFFER_PRINT_RAW_CHARS(n)\
	do {\
		printf("\nraw buffer contents up to %lu:\n", (size_t)(n));\
		int i = 0;\
		while (i < (n)) {\
			if (buffer[i] == '\0') { putchar('@'); }\
			else putchar(buffer[i]);\
			++i;\
		}\
	} while(0)

#define PRINT_RAW_CHARS(buf, n)\
	do {\
		printf("\nraw buffer contents up to %lu:\n", (size_t)(n));\
		int i = 0;\
		while (i < (n)) {\
			if ((buf)[i] == '\0'){ putchar('@'); }\
			else putchar((buf)[i]);\
			++i;\
		}\
	} while(0)

#define HIST_CURRENT_INCREMENT()\
	do {\
		hist_current = hist_current->next ? hist_current->next : &buffer_current;\
	} while(0)

#define HIST_CURRENT_DECREMENT()\
	do {\
			hist_current = hist_current->prev ? hist_current->prev : hist_root;\
	} while(0)


#define CTRL_A		0x1
#define CTRL_E		0x5
#define CTRL_K		0xB
#define CTRL_E		0x5
#define LINE_FEED 	'\n'
#define TAB_STOP	'\t'
#define BACKSPACE	0x7F

// ANSI X3.64 arrow keypress control sequence (0x1B5Bxx) last byte identifiers
// (there are longer ones as well, e.g. the F9-F12 keys generate a total of 5 bytes)

#define ARROW_UP 	0x41
#define ARROW_DOWN 	0x42	
#define ARROW_LEFT 	0x44
#define ARROW_RIGHT 	0x43	

#define DELETE		0x33

void rl_emul_init();
void rl_emul_deinit();
int rl_emul_initialized();

// these are really the only two features we need

char *rl_emul_readline(const char* prompt);
void rl_emul_hist_add(const char *term);

#endif
