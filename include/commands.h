#ifndef COMMANDS_H
#define COMMANDS_H

#include <stdlib.h>
#include <stdio.h>
#include "definitions.h"
#include "functions.h"
#include "ud_constants_tree.h"
#include "wlist.h"

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

#endif
