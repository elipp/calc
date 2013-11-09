#ifndef COMMANDS_H
#define COMMANDS_H

#include <stdlib.h>
#include <stdio.h>
#include "fp_t.h"
#include "functions.h"
#include "ud_constants_tree.h"
#include "wlist.h"

typedef struct {
	const char* key;
	void (*funcptr)(fp_t*, const char*);
} key_strfuncptr_pair;

typedef struct {
	const char *key;
	void (*func)(struct wlist_t*);	// the argument is always either a NULL or a wlist*
} key_funcptr_pair;

void help(struct wlist_t* wlist);
void help_functions();
void help_constants();
void help_my();
void help_set();

void set(struct wlist_t* wlist);
void set_precision(long p);

void my(struct wlist_t* wlist);
void my_list();

void quit();

extern const key_funcptr_pair commands[];
extern const size_t commands_table_size;

extern int precision_bits;
extern int print_significant_figures;

#endif
