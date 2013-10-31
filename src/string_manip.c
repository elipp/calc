#include "string_manip.h"
#include "string_allocator.h"
#include "functions.h"

char *strip_all_whitespace(const char *str) {

	size_t str_len = strlen(str);

	char *stripped = sa_alloc(str_len); // will allocate more memory than needed

	int i = 0;
	int index = 0;
	for (; i < str_len; ++i) {
		if (str[i] != ' ' && str[i] != '\n') stripped[index++] = str[i];
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
	
	char *stripped = substring(str, 1, str_len-2);
	return stripped; 
}

char *strip_surrounding_whitespace(const char* str) {
	size_t str_len = strlen(str);

	int beg = 0;
	while (beg < str_len) {
		if (str[beg] != ' ') break;
		++beg;
	}

	int end = str_len-1;
	while (end >= 0) {
		if (str[end] != ' ') break;
		--end;
	}
	
	int len = end - beg + 1;
	if (len < 1) {
		fprintf(stderr, "warning: strip_surrounding_whitespace: root string len < 1!\n");
		return NULL;
	}

	char *r = substring(str, beg, len);
	return r;
}

char *strip_duplicate_whitespace(const char* str) {

	size_t str_len = strlen(str);
	char *s = sa_alloc(str_len); // generally, this will allocate more memory than we actually need
	memset(s, '\0', str_len);
	
	int i = 0;
	int index = 0;
	while (i < str_len) {
		s[index] = str[i];
		++index;
		if (str[i] == ' ') {
			while (i+1 < str_len && str[i+1] == ' ') ++i;
		}
		++i;
	}

	return s;
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

