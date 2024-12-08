#include "da.h"
#include "utils.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Pair_int {
	int first;
	int second;
};

/**
 * Converts each `char*` to a `struct Pair_int`, within the given index range.
 *
 * The number of elements converted will be equal to `end - begin`.
 *
 * @param [inout] dst	[elem_type -> struct Pair_int]
 * @param [inout] src	[elem_type -> char*]
 * @param         begin	start index (inclusive)
 * @param         end	end index (exclusive)
 */
void convert_to_rules(da_type* dst, da_type* src, size_t begin, size_t end);

/**
* Converts each `char*` to a DynamicArray of int, within the given index range.
*
* The number of elements converted will be equal to `end - begin`.
*
* @param [inout] dst	[elem_type -> da_type [elem_type -> int]]
* @param [inout] src	[elem_type -> char*]
* @param         begin	start index (inclusive)
* @param         end	end index (exclusive)
 */
void convert_to_page_lists(
	da_type* dst, da_type* src, size_t begin, size_t end
);

/**
 * Verifies that the pages are in order, according to the rules.
 *
 * @param [in]    page_list
 * @param [in]    rules
 *
 * @returns	0 on success
 * @returns	non-zero on failure
 */
int verify_pages(da_type* page_list, da_type* rules);

/**
 * Sorts the page list so, according to the rules.
 *
 * @param [inout] page_list
 * @param [in]    rules
 *
 * @returns	0 on success
 * @returns	non-zero on failure
 */
int sort_pages(da_type* page_list, da_type* rules);

int main(void) {
	int rv = 0;
	da_type* lines = da_create(sizeof(char*));
	da_type* rules = da_create(sizeof(struct Pair_int));
	da_type* page_lists = da_create(sizeof(da_type*));

	size_t i = 0;
	size_t break_point = 0;
	size_t sum1 = 0;
	size_t sum2 = 0;

	da_set_free_fn(lines, &free);
	da_set_free_fn(page_lists, &da_destroy);

	read_lines("./data/test.txt", lines);

	/* find blank line splitting data */
	for (i = 0; i < da_size(lines); ++i) {
		char* s = *(char**)da_at(lines, i);
		if (strlen(s) == 0) {
			break_point = i;
			break;
		}
	}
	if (break_point == 0) {
		fprintf(stderr, "blank line not found\n");
		rv = 1;
		goto exit;
	}
	printf("break point at line %li\n\n", break_point + 1);

	/* convert data */
	convert_to_rules(rules, lines, 0, break_point);
	convert_to_page_lists(
		page_lists, lines, break_point + 1, da_size(lines)
	);

	/* verify page lists */
	for (i = 0; i < da_size(page_lists); ++i) {
		char* s = NULL;
		size_t mid = 0;
		da_type* list = *(da_type**)da_at(page_lists, i);

		printf("Verifying page list: ");
		da_print(list, "%i", int);

		/* all rules ok */
		if (verify_pages(list, rules) == 0) {
			printf("ok!\n");
			mid = da_size(list) / 2;
			sum1 += *(int*)da_at(list, mid);
		} else {
			printf("Sorting page list...\n");

			/* some lists may need multiple passes */
			/* hopefully all page lists _can_ be sorted... */
			do {
				sort_pages(list, rules);
			} while (verify_pages(list, rules) != 0);

			da_print(list, "%i", int);
			mid = da_size(list) / 2;
			sum2 += *(int*)da_at(list, mid);
		}

		printf("\n");

		free(s);
	}

	printf("Output 1 -> %lu\n", sum1);
	printf("Output 2 -> %lu\n", sum2);

exit:
	da_destroy(page_lists);
	da_destroy(rules);
	da_destroy(lines);

	return rv;
}

void convert_to_rules(da_type* dst, da_type* src, size_t begin, size_t end) {
	size_t i = 0;
	struct Pair_int pair = {0};

	for (i = begin; i < end; ++i) {
		char* s = NULL;
		/* strtok modifies the string, so take a copy */
		char* in_copy = *(char**)da_at(src, i);
		char* str = malloc(strlen(in_copy) + 1);
		strcpy(str, in_copy);

		s = strtok(str, "|");
		pair.first = atoi(s);
		s = strtok(NULL, "|");
		pair.second = atoi(s);

		da_append(dst, &pair);
		free(str);
	}
}

void convert_to_page_lists(
	da_type* dst, da_type* src, size_t begin, size_t end
) {
	size_t i = 0;
	da_type* da = NULL;

	for (i = begin; i < end; ++i) {
		char* s = NULL;
		/* strtok modifies the string, so take a copy */
		char* in_copy = *(char**)da_at(src, i);
		char* str = malloc(strlen(in_copy) + 1);
		strcpy(str, in_copy);

		da = da_create(sizeof(int));
		s = strtok(str, ",");
		while (s != NULL) {
			int n = atoi(s);
			da_append(da, &n);
			s = strtok(NULL, ",");
		}

		da_append(dst, &da);
		free(str);
	}
}

int verify_pages(da_type* list, da_type* rules) {
	size_t i = 0;
	void* ptr_a = NULL;
	void* ptr_b = NULL;

	for (i = 0; i < da_size(rules); ++i) {
		struct Pair_int* pair = da_at(rules, i);
		ptr_a = da_find(
			list, da_begin(list), da_end(list), &pair->first
		);
		ptr_b = da_find(
			list, da_begin(list), da_end(list), &pair->second
		);

		/* error */
		if (ptr_a == NULL || ptr_b == NULL) {
			continue;
		}

		/* no match */
		if (ptr_a == da_end(list) || ptr_b == da_end(list)) {
			continue;
		}


		if (ptr_a > ptr_b) {
			printf(
				"failed on rule %i|%i\n",
				pair->first, pair->second
			);
			return 1;
		}
	}

	/* all rules ok */
	return 0;
}

int sort_pages(da_type* list, da_type* rules) {
	size_t i = 0;
	void* ptr_a = NULL;
	void* ptr_b = NULL;

	for (i = 0; i < da_size(rules); ++i) {
		struct Pair_int* pair = da_at(rules, i);
		ptr_a = da_find(
			list, da_begin(list), da_end(list), &pair->first
		);
		ptr_b = da_find(
			list, da_begin(list), da_end(list), &pair->second
		);

		/* error */
		if (ptr_a == NULL || ptr_b == NULL) {
			continue;
		}

		/* no match */
		if (ptr_a == da_end(list) || ptr_b == da_end(list)) {
			continue;
		}

		/* move "b" just after "a" */
		if (ptr_a > ptr_b) {
			/* "insert" may invalidate pointers */
			char* old = da_begin(list);
			char* new = NULL;

			da_insert(list, ptr_a, ptr_b);
			new = da_begin(list);

			if (new != old) {
				ptr_b = (char*)ptr_b + (old - new);
			}

			da_erase(list, ptr_b);
			/**/
			printf("rule %i|%i -> ", pair->first, pair->second);
			da_print(list, "%i", int);
		}
	}

	/* all rules ok */
	return 0;
}
