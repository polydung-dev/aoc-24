#include "da.h"
#include "str.h"
#include "utils.h"

#include <assert.h>
#include <limits.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char filename[] = "./data/07/test.txt";

struct CalibrationEquation {
	uint64_t target;
	da_type* operands;
};

#define oom()                                \
do {                                         \
	fprintf(stderr, "out of memory!\n"); \
	exit(1);                             \
} while (0)                                  \

void print_bits(uint64_t n, size_t num_bits, int do_space) {
	size_t i = 0;
	for (i = num_bits; i --> 0; /**/ ) {
		printf("%ld", (n >> i) & 1);

		if (do_space && (i % 8) == 0) {
			printf(" ");
		}
	}
}

char* to_bits(uint64_t n, size_t num_bits) {
	size_t i = 0;
	char* c = malloc(num_bits * sizeof(char) + 1);
	char* p = c;

	if (c == NULL) { oom(); }

	for (i = num_bits; i --> 0; /**/ ) {
		sprintf(p, "%c", ((n >> i) & 1) ? '1' : '0');
		++p;
	}

	return c;
}

void convert_data(da_type* dst, da_type* src);

struct DAPrinterConfig printer_config = {0};
void equation_printer(da_type* da, size_t index);
void equation_destructor(void* ptr);

uint64_t test(struct CalibrationEquation* eq, char* ops);

int main(void) {
	size_t i = 0;
	uint64_t sum = 0;
	da_type* lines = da_create(sizeof(char*));
	da_type* equations = da_create(sizeof(struct CalibrationEquation*));
	printer_config.printer = &equation_printer;

	da_set_destructor(lines, &free);
	da_set_destructor(equations, &equation_destructor);
	read_lines(filename, lines);

	convert_data(equations, lines);
	da_destroy(lines);

	for (i = 0; i < da_size(equations); ++i) {
		size_t j = 0;
		struct CalibrationEquation* e = NULL;
		e = da_get_as(equations, i, struct CalibrationEquation*);

		printf("Calibrating...\n");
		equation_printer(equations, i);
		printf("\n");

		for (j = 0; j < pow(2, da_size(e->operands) - 1); ++j) {
			char* ops = to_bits(j, da_size(e->operands) - 1);
			uint64_t res = test(e, ops);
			if (res == e->target) {
				sum += res;
				printf("%s\n", ops);
				free(ops);
				break;
			}

			free(ops);
		}

		printf("\n");
	}

	printf("Total Calibration Result: %lu\n", sum);

	da_destroy(equations);
	return 0;
}

void convert_data(da_type* dst, da_type* src) {
	size_t i = 0;
	for (i = 0; i < da_size(src); ++i) {
		size_t j = 0;
		char* line = *(char**)da_at(src, i);
		da_type* parts = NULL;
		da_type* operands = NULL;
		struct CalibrationEquation* e = NULL;

		parts = str_split(line, ":");

		e = malloc(sizeof(*e));
		if (e == NULL) { oom(); }
		e->target = atol(*(char**)da_at(parts, 0));
		operands = str_split(*(char**)da_at(parts, 1), " ");

		e->operands = da_create(sizeof(int));
		for (j = 0; j < da_size(operands); ++j) {
			int n = atoi(*(char**)da_at(operands, j));
			da_append(e->operands, &n);
		}
		da_append(dst, &e);

		da_destroy(parts);
		da_destroy(operands);

	}
}

void equation_printer(da_type* da, size_t index) {
	size_t i = 0;
	struct CalibrationEquation* e = NULL;
	e = da_get_as(da, index, struct CalibrationEquation*);

	printf("%6lu : ", e->target);
	for (i = 0; i < da_size(e->operands); ++i) {
		printf("%2i ", *(int*)da_at(e->operands, i));
	}
}

void equation_destructor(void* ptr) {
	if (ptr == NULL) {
		return;
	}

	da_destroy(((struct CalibrationEquation*)ptr)->operands);
	free(ptr);
}


uint64_t test(struct CalibrationEquation* eq, char* ops) {
	size_t i = 0;
	size_t count = strlen(ops);
	uint64_t result = da_get_as(eq->operands, i, int);

	for(i = 0; i < count; ++i) {
		int b = da_get_as(eq->operands, i + 1, int);
		/* printf("%lu; ", result); */
		switch (ops[i]) {
			case '0':
				/* printf("+"); */
				result += b; break;
			case '1':
				/* printf("*"); */
				result *= b; break;
		}

		/* printf(" %i -> %lu; ", b, result); */
	}


	/* printf("\n"); */

	return result;
}
