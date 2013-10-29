#include "string_manip.h"
#include "string_allocator.h"
#include "mathstuff.h"

char *strip_all_whitespace(const char *str) {

	size_t str_len = strlen(str);

	char *stripped = sa_alloc(str_len); // will allocate more memory than needed

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

	char *sub = sa_alloc(nc+1);

	int i = 0;
	for (; i < nc; ++i) {
		sub[i] = str[beg_pos+i];
	}

	sub[nc] = '\0';

	return sub;
}

char *strip_surrounding_parentheses(const char* str) {
	size_t str_len = strlen(str);
	char *stripped = sa_alloc(str_len - 2 + 1);	// -2 for the two parentheses to be deleted, +1 for '\0' :)

	strncpy(stripped, str+1, str_len-2);
	stripped[str_len-2] = '\0';

	return stripped; 
}

int check_alphanumeric_validity(const char* str) {
	size_t str_len = strlen(str);
	
	int i = 0;
	for (; i < str_len; ++i) {
		char c = str[i];
		if (!(ALPHANUMERIC(c) || valid_math_char(c))) {
			fprintf(stderr, "Syntax error: invalid character input \'%c\' @ index %d \n", c, i);
			return 0;
		}
	}
	return 1;
}

