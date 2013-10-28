#include "string_manip.h"

char *strip_all_whitespace(const char *str) {

	size_t str_len = strlen(str);

	char *stripped = malloc(str_len); // will allocate more memory than needed

	int i = 0;
	int index = 0;
	for (; i < str_len; ++i) {
		if (str[i] != ' ') stripped[index++] = str[i];
	}

	stripped[index] = '\0';

	return stripped;
}

char *substring(const char* str, int beg_pos, int nc) {

	size_t str_len = strlen(str);
	if (!str_len) { return NULL; }

	if (str_len < beg_pos + nc) {
		nc = str_len - beg_pos;
	}

	char *sub = malloc(nc+1);

	int i = 0;
	for (; i < nc; ++i) {
		sub[i] = str[beg_pos+i];
	}

	sub[nc] = '\0';

	return sub;
}

char *strip_outer_braces(const char* str) {
	size_t str_len = strlen(str);
	char *stripped = malloc(str_len - 2 + 1);

	strncpy(stripped, str+1, str_len-2);
	stripped[str_len-2] = '\0';

	return stripped; 
}


