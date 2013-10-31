#ifndef COMMANDS_H
#define COMMANDS_H

#include <stdlib.h>
#include <stdio.h>
#include "definitions.h"
#include "functions.h"
#include "ud_constants_tree.h"
#include "wlist.h"

typedef struct {
	const char* key;
	fp_t (*funcptr)(const char*);
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
void set_precision();

void my(struct wlist_t* wlist);
void my_list();

void quit();

extern const key_funcptr_pair commands[];
extern const size_t commands_table_size;

#endif
