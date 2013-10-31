#include <string.h>

#include "wlist.h"
#include "string_manip.h"
#include "string_allocator.h"
#include "commands.h"
#include "tables.h"

static void wlist_resize(struct wlist_t *wlist) {
	wlist->capacity *= 2;
	wlist->strings = realloc(wlist->strings, sizeof(wlist->strings[0])*wlist->capacity);
}

static struct wlist_t wlist_create() {
	struct wlist_t wl;
	wl.capacity = 16;
	wl.num_words = 0;
	wl.total_length = 0;
	wl.strings = malloc(sizeof(char*)*wl.capacity);

	return wl;
}

static void wlist_add_word(struct wlist_t *wlist, const char* arg) {
	if (wlist->num_words >= wlist->capacity) {
		wlist_resize(wlist);
	}

	size_t str_len = strlen(arg);
	
	wlist->strings[wlist->num_words] = sa_strdup(arg);
	++wlist->num_words;
	wlist->total_length += str_len;

}

struct wlist_t wlist_generate(const char* arg, const char* delims) {

	struct wlist_t wlist = wlist_create();
	char *dup = sa_strdup(arg);
	char *t = strtok(dup, delims);
	while (t != NULL) {
		wlist_add_word(&wlist, t);
		t = strtok(NULL, delims);
	}
	sa_free(dup);
	return wlist;
}

void wlist_print(struct wlist_t *list) {
	int i = 0;
	for (; i < list->num_words; ++i) {
		fprintf(stderr, "\"%s\"\n", list->strings[i]);
	}
}

char *wlist_recompose(struct wlist_t *list) {
	// recompose with ' ' as delimiter
	size_t total_length = list->total_length + (list->num_words);	// a space character to be put between each word
	char *str = sa_alloc(total_length*sizeof(char));
	memset(str, '\0', total_length);

	int i = 0;
	for (; i < list->num_words; ++i) {
		strcat(str, list->strings[i]);
		strcat(str, " ");
	}

	str[total_length-1] = '\0';
	str = strip_surrounding_whitespace(strip_duplicate_whitespace(str));

	return str;
}

void wlist_destroy(struct wlist_t *wlist) {
	// sa_alloc allocated strings dont need to be freed (sa_free is a no-op), so just free the ptr array
	free(wlist->strings);
	wlist->capacity = 0;
	wlist->num_words = 0;
	wlist->total_length = 0;
}

int wlist_parse_command(struct wlist_t *list) {
	
	int i = 0;
	for (; i < commands_table_size; ++i) {
		if (strcmp(list->strings[0], commands[i].key) == 0) {
			commands[i].func(list);
			return 1;
		}
	}
	return 0;
}
