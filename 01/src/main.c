#include "da.h"

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct DA_int {
	int* data;
	size_t size;
	size_t capacity;
};

void load_from_file(
	const char* path, struct DA_int* left, struct DA_int* right
);
void split_line(char* buf, int* a, int* b);
int cmp_int(const void* a, const void* b);

int main(void) {
	size_t i = 0;
	unsigned long sum = 0;
	struct DA_int* left = NULL;
	struct DA_int* right = NULL;

	da_create(left);
	da_create(right);

	load_from_file("./data/test.txt", left, right);

	qsort(left->data, left->size, sizeof(left->data[0]), &cmp_int);
	qsort(right->data, right->size, sizeof(right->data[0]), &cmp_int);

	assert(left->size == right->size);
	sum = 0;
	for (i = 0; i < left->size; ++i) {
		int diff = abs(left->data[i] - right->data[i]);
		sum += diff;
	}

	printf("Sum of differences: %lu\n", sum);

	da_destroy(right);
	da_destroy(left);

	return 0;
}

void load_from_file(
	const char* path, struct DA_int* left, struct DA_int* right
) {
	size_t buf_sz = 256;
	char* buf = NULL;
	FILE* fp = NULL;

	buf = malloc(buf_sz);
	if (buf == NULL) {
		fprintf(stderr, "out of memory\n");
		exit(1);
	}

	fp = fopen(path, "r");
	if (fp == NULL) {
		fprintf(
			stderr, "Could not open file `%s`: %s\n",
			path, strerror(errno)
		);
		exit(1);
	}

	while (fgets(buf, buf_sz, fp) != NULL) {
		int a, b;
		split_line(buf, &a, &b);
		da_append(left, a);
		da_append(right, b);
	}

	fclose(fp);
	free(buf);
}

void split_line(char* buf, int* a, int* b) {
	char* s = NULL;

	s = strtok(buf, " ");
	*a = atoi(s);
	s = strtok(NULL, " ");
	*b = atoi(s);
}

int cmp_int(const void* a, const void* b) {
	const int* x = a;
	const int* y = b;

	if (*x == *y) {
		return 0;
	}

	return (*x < *y) ? -1 : 1;
}
