#include "da.h"
#include "utils.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Rule {
	int first;
	int second;
};

struct DA_rule {
	struct Rule* data;
	size_t size;
	size_t capacity;
};

struct DA_int {
	int* data;
	size_t size;
	size_t capacity;
};

struct DA_da_int {
	struct DA_int** data;
	size_t size;
	size_t capacity;
};

/**
 * Split array in two at the index.
 *
 * @param [inout] dst	will contain elements from `index` onwards
 * @param [inout] src	will contain elements up to (but not including) `index`
 */
void split_array(struct DA_string* dst, struct DA_string* src, size_t index);

void convert_to_rules(
	struct DA_rule* rules, struct DA_string* src,
	size_t begin, size_t end
);
void convert_to_page_lists(
	struct DA_da_int* page_lists, struct DA_string* src,
	size_t begin, size_t end
);

/**
 * Verifies that the pages are in order, according to the rules.
 *
 * @returns	0 on success
 * @returns	non-zero on failure
 */
int verify_pages(struct DA_int* page_list, struct DA_rule* rules);

int main(void) {
	struct DA_string* lines = NULL;
	struct DA_rule*   rules = NULL;
	struct DA_da_int* page_lists = NULL;
	size_t i = 0;
	size_t break_point = 0;
	size_t sum = 0;

	da_create(lines);
	read_lines("./data/test.txt", lines);

	/* find blank line splitting data */
	for (i = 0; i < lines->size; ++i) {
		if (strlen(lines->data[i]) == 0) {
			break_point = i;
			break;
		}
	}
	if (break_point == 0) {
		fprintf(stderr, "blank line not found\n");
		exit(1);
	}

	printf("break point at line %li\n", break_point + 1);

	/* convert data */
	da_create(rules);
	convert_to_rules(rules, lines, 0, break_point);
	da_create(page_lists);
	convert_to_page_lists(page_lists, lines, break_point + 1, lines->size);

	/* verify page lists */
	for (i = 0; i < page_lists->size; ++i) {
		char* s = NULL;
		size_t mid = 0;
		struct DA_int* list = page_lists->data[i];

		printf("Verifying page list: ");
		da_print(list, "%i");

		/* all rules ok */
		if (verify_pages(list, rules) == 0) {
			printf("ok!\n");
			mid = list->size / 2;
			sum += list->data[mid];
		}

		printf("\n");

		free(s);
	}

	printf("Output -> %lu\n", sum);

	da_destroy_fn(page_lists, da_destroy);
	da_destroy(rules);
	da_destroy_fn(lines, free);
	return 0;
}

void split_array(struct DA_string* dst, struct DA_string* src, size_t index) {
	size_t count = src->size - index;

	/* copy string pointers over */
	dst->size = count;
	da_resize(dst, count);
	memcpy(dst->data, src->data + index, count*sizeof(src->data[0]));

	/* remove from old array */
	src->size -= (count);
	memset(src->data + index, 0, count);
	da_resize(src, count);
}

void convert_to_rules(
	struct DA_rule* rules, struct DA_string* src,
	size_t begin, size_t end
) {
	size_t i = 0;
	struct Rule rule = {0};

	for (i = begin; i < end; ++i) {
		char* s = NULL;
		/* strtok modifies the string */
		char* str = malloc(strlen(src->data[i]) + 1);
		strcpy(str, src->data[i]);

		s = strtok(str, "|");
		rule.first = atoi(s);
		s = strtok(NULL, "|");
		rule.second = atoi(s);

		da_append(rules, rule);
		free(str);
	}
}
void convert_to_page_lists(
	struct DA_da_int* page_lists, struct DA_string* src,
	size_t begin, size_t end
) {

	size_t i = 0;
	struct DA_int* da = NULL;

	for (i = begin; i < end; ++i) {
		char* s = NULL;
		/* strtok modifies the string */
		char* str = malloc(strlen(src->data[i]) + 1);
		strcpy(str, src->data[i]);

		da_create(da);

		s = strtok(str, ",");
		while (s != NULL) {
			da_append(da, atoi(s));
			s = strtok(NULL, ",");
		}

		da_append(page_lists, da);
		free(str);
	}
}

int verify_pages(struct DA_int* list, struct DA_rule* rules) {
	size_t i = 0;
	int idx_a = -1;
	int idx_b = -1;

	/* check each rule one by one */
	for (i = 0; i < rules->size; ++i) {
		da_find(list, rules->data[i].first, idx_a);
		da_find(list, rules->data[i].second, idx_b);

		/* no match */
		if (idx_a == -1 || idx_b == -1) {
			continue;
		}

		if (idx_a > idx_b) {
			printf(
				"failed on rule %i|%i\n",
				rules->data[i].first,
				rules->data[i].second
			);
			return 1;
		}

	}

	/* all rules ok */
	return (i != rules->size);
}
