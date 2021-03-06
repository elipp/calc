#include "ud_constants_tree.h"
#include "fp_t.h"
#include "functions.h"

// not implementing udc_tree as a struct, since there will only ever be 1 of them
// Instead, it is implemented as a collection of global variables :-D

static struct udc_node *udctree_head;
static struct udc_node *udctree_root;
static size_t udctree_num_nodes;

static void udctree_init() {
	udctree_head = NULL;
	udctree_root = NULL;
	udctree_num_nodes = 0;
}

static struct udc_node *udc_node_create(const char* term, fp_t value) {
	struct udc_node *node = malloc(sizeof(struct udc_node));
	node->pair.key = strdup(term);
	fp_t_construct(&node->pair.value);
	fp_t_assign(&node->pair.value, value);
	node->next = NULL;
	return node;
}


struct udc_node *udctree_add(const char* key, fp_t value) {

	struct udc_node *node = udc_node_create(key, value);
	if (udctree_num_nodes == 0) {
		udctree_init();
		udctree_head = node;
		udctree_root = node;
		udctree_head->next = NULL;
	}
	else {
		udctree_head->next = node;
		udctree_head = node;
		node->next = NULL;
	}
	++udctree_num_nodes;
	return node;

}


void udctree_destroy() {

	if (udctree_num_nodes > 0) {
		struct udc_node *iter = udctree_get_root();
		struct udc_node *nexttmp = iter->next;
		while (nexttmp != NULL) {
			free(iter->pair.key);
			free(iter);
			iter = nexttmp;
			nexttmp = nexttmp->next;
		}
		free(iter->pair.key);
		free(iter);
	}
}

size_t udctree_get_num_nodes() { return udctree_num_nodes; }

struct udc_node *udctree_search(const char* term) {
	if (udctree_num_nodes == 0) { return NULL; }
	struct udc_node *iter = udctree_get_root();
	while (iter != NULL) {
		if (strcmp(term, iter->pair.key) == 0) { return iter; }
		iter = iter->next;
	}
	return NULL;
	
}

struct udc_node *udctree_get_root() { return udctree_root; } 
