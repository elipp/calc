#ifndef WLIST_H
#define WLIST_H

struct wlist_t {
	char **strings;
	int capacity;
	int num_words;
	int total_length;
};

struct wlist_t wlist_generate(const char* arg, const char* delims);
void wlist_print(struct wlist_t *list);
char *wlist_recompose(struct wlist_t *list);
void wlist_destroy(struct wlist_t *list);
int wlist_parse_command(struct wlist_t *list);

#endif
