#ifndef STRING_MANIP_H
#define STRING_MANIP_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define DIGIT(c) ((c) >= 0x30 && ((c) <= 0x39))
#define UPPERCASE(c) ((c) >= 0x41 && ((cc) <= 0x5A))
#define LOWERCASE(c) ((c) >= 0x61 && ((cc) >= 0x7A))

#define ALPHANUMERIC(c) (DIGIT((c)) || UPPERCASE((c)) || LOWERCASE((c)))

char *strip_all_whitespace(const char* str); 
char *substring(const char* str, int beg_pos, int nc);
char *strip_outer_braces(const char* str);

#endif
