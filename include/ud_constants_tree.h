#ifndef UD_CONSTANTS_TREE_H
#define UD_CONSTANTS_TREE_H
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "definitions.h"
#include "tables.h"

struct udc_node {
	struct udc_node *next;
	key_constant_pair pair;
};

struct udc_node *udctree_add(const char* key, fp_t value);
void udctree_delete();
size_t udctree_get_num_nodes();
struct udc_node *udctree_search(const char* term);
struct udc_node *udctree_get_root();
key_constant_pair *udctree_match(const char* term);

#endif
