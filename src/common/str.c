#include "da.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

da_type* str_split(char* str, char* delim) {
	da_type* out = da_create(sizeof(char*));
	char* tmp = malloc(strlen(str) + 1);
	char* s = NULL;
	char* t = NULL;

	da_set_destructor(out, &free);

	if (tmp == NULL) {
		fprintf(stderr, "out of memory\n");
		da_destroy(out);
		exit(1);
	}

	/* strtok modifies it's input */
	strcpy(tmp, str);
	s = strtok(tmp, delim);
	while (s != NULL) {
		t = malloc(strlen(s) + 1);
		if (t == NULL) {
			fprintf(stderr, "out of memory\n");
			da_destroy(out);
			free(tmp);
			exit(1);
		}
		strcpy(t, s);
		da_append(out, &t);
		s = strtok(NULL, delim);
	}

	free(tmp);

	return out;
}

char* str_join(char* joiner, char** strings, size_t count) {
	size_t i = 0;
	size_t sz = 1 + ((count - 1) * strlen(joiner));
	char* s = NULL;
	char* p = NULL;

	for (i = 0; i < count; ++i) {
		sz += strlen(strings[i]);
	}

	s = calloc(sz, 1);
	p = s;

	for (i = 0; i < count; ++i) {
		p = strcpy(p, strings[i]);
	}

	return s;
}

void str_r_strip(char* string, char* to_strip) {
	char* r = NULL;
	char* c = NULL;

	/* for each char in string (from the end) */
	for (r = string + strlen(string) - 1; r >= string; --r) {
		/* for each char to strip */
		for (c = to_strip; *c != '\0'; ++c) {
			/* strip char if equal */
			if (*r == *c) {
				*r = '\0';
				break;
			}
		}
		/* stop at first non-strip char */
		if (*c == '\0') {
			break;
		}
	}
}
