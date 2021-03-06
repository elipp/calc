#include "string_manip.h"
#include "string_allocator.h"
#include "functions.h"

char *strip_all_whitespace(const char *str) {

	if (!str) { return NULL; }
	size_t str_len = strlen(str);

	char *stripped = sa_alloc(str_len+1); // will allocate more memory than needed

	int i = 0;
	int index = 0;
	for (; i < str_len; ++i) {
		if (str[i] != ' ' && str[i] != '\n') stripped[index++] = str[i];
	}

	stripped[index] = '\0';

	return stripped;
}

char *substring(const char* str, int beg_pos, int nc) {

	if (!str) { return NULL; }
	size_t str_len = strlen(str);
	if (!str_len) { return NULL; }

	if (str_len < beg_pos + nc) {
		nc = str_len - beg_pos;
	}

	char *sub = sa_alloc(nc+1);

	/*
	int i = 0;
	for (; i < nc; ++i) {
		sub[i] = str[beg_pos+i];
	} 
	*/
	strncpy(sub, str+beg_pos, nc);

	sub[nc] = '\0';

	return sub;
}

char *strip_surrounding_parentheses(const char* str) {

	if (!str) { return NULL; }
	size_t str_len = strlen(str);
	
	char *stripped = substring(str, 1, str_len-2);
	return stripped; 
}

char *strip_surrounding_whitespace(const char* str) {

	if (!str) { return NULL; }
	size_t str_len = strlen(str);

	int beg = 0;
	for (; beg < str_len; ) {
		if (str[beg] == ' ') ++beg;
		else break;
	}

	int end = str_len-1;
	for (; end > 0; ) {
		if (str[end] == ' ') --end;
		else break;
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

	if (!str) { return NULL; }

	size_t str_len = strlen(str);
	char *r = sa_alloc(str_len+1); // generally, this will allocate more memory than we actually need
	
	int i = 0;
	int index = 0;
	while (i < str_len) {
		r[index] = str[i];
		++index;
		if (str[i] == ' ') {
			while (i+1 < str_len && str[i+1] == ' ') ++i;
		}
		++i;
	}

	r[index] = '\0';

	return r;
}

int check_alphanumeric_validity(const char* str) {

	if (!str) { return 1; }
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

