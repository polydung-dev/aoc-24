#include "utils.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void read_lines(const char* path, struct DA_string* lines) {
	size_t buf_sz = 256;
	char* buf = NULL;
	FILE* fp = NULL;

	fp = fopen(path, "r");
	if (fp == NULL) {
		fprintf(
			stderr, "Could not open file `%s`: %s\n",
			path, strerror(errno)
		);
		exit(1);
	}

	buf = malloc(buf_sz);
	if (buf == NULL) {
		fprintf(stderr, "out of memory\n");
		fclose(fp);
		free(buf);
		exit(1);
	}

	while (fgets(buf, buf_sz, fp) != NULL) {
		char* s = NULL;

		if ((strlen(buf) + 1) == buf_sz) {
			fprintf(stderr, "buffer too small\n");
			fclose(fp);
			free(buf);
			exit(1);
		}

		/* strings must be free'd by the caller */
		s = malloc(strlen(buf) + 1);
		strcpy(s, buf);
		r_strip(s, "\n");

		da_append(lines, s);
	}

	fclose(fp);
	free(buf);
}

void r_strip(char* string, char* to_strip) {
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

char* join(char* joiner, char** strings, size_t count) {
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
