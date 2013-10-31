#ifndef RL_EMUL_H
#define RL_EMUL_H

#include <string.h>
#include <termios.h>
#include <stdlib.h>
#include <stdio.h>

#define IS_REGULAR_INPUT(x) ((x) > 0x19 && (x) < 0x7F)

void rl_emul_init();
void rl_emul_deinit();

// these are really the only two features we need

char *rl_emul_readline(const char* prompt);
void rl_emul_hist_add(const char *term);

#endif
