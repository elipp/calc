#ifndef STRING_MANIP_H
#define STRING_MANIP_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define DIGIT(c) ((c) >= '0' && ((c) <= '9'))
#define UPPERCASE(c) ((c) >= 'A' && ((c) <= 'Z'))
#define LOWERCASE(c) ((c) >= 'a' && ((c) <= 'z'))

#define LETTER_CHAR_ANY(c) (UPPERCASE((c)) || LOWERCASE((c)))
#define ALPHANUMERIC(c) (DIGIT((c)) || UPPERCASE((c)) || LOWERCASE((c)))

char *strip_all_whitespace(const char* str); 
char *substring(const char* str, int beg_pos, int nc);
char *strip_surrounding_parentheses(const char* str);
char *strip_surrounding_whitespace(const char* str);
int check_alphanumeric_validity(const char *str);

#endif
