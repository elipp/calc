#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "string_allocator.h"

#define SA_POOLSIZE (8*1024)

static char sa_buffer[SA_POOLSIZE];
static int sa_curindex = 0;

void sa_clearbuf() {
	memset(sa_buffer, 0, sizeof(sa_buffer));
	sa_curindex = 0;
}

void *sa_alloc(size_t size) {
	size_t num_chars = size/(sizeof(char));	// just to be on the safe side :P

	if (sa_curindex + num_chars > SA_POOLSIZE) {
		fprintf(stderr, "sa_alloc: out-of-memory error (sa_curindex = %d)\n", sa_curindex);
		return NULL;
	}

	char *ptr = sa_buffer + sa_curindex;
	sa_curindex += num_chars + 1;	// +1 for the '\0' char

	return (void*)ptr;
}

void sa_free(void *mem) {};
