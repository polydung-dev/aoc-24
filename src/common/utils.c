#include "utils.h"
#include "str.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void read_lines(const char* path, da_type* lines) {
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
		str_r_strip(s, "\n");

		da_append(lines, &s);
	}

	fclose(fp);
	free(buf);
}
