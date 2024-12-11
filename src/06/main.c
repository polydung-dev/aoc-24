#include "da.h"
#include "utils.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char filename[] = "./data/06/test.txt";

da_printer_fn string_printer;
struct DAPrinterConfig string_printer_conf = {
	&string_printer, NULL, "\n", "\n", NULL
};

#define cursor_up    '^'
#define cursor_left  '<'
#define cursor_down  'v'
#define cursor_right '>'
#define wall_char    '#'

/**
 * @param [in]    da
 * @param         c 	char to find
 * @param [out]   x 	x index (column) of character
 * @param [out]   y 	y index (row) of character
 */
void find_char(da_type* da, char c, size_t* x, size_t* y);

/**
 * Advances the cursor according to the rules and marks the path as visited.
 *
 * Rules:
 * - attempt to move forward
 * - turn 1/4 turn clockwise upon failure
 *
 * @param [inout] da
 * @param [inout] x
 * @param [inout] y
 *
 * @return	0 if move was successful
 * @return	1 if move would be out of bounds
 */
int advance_cursor(da_type* da, size_t* x, size_t* y);

/**
 * Counts the number of times the given char appears.
 *
 * @param [in]    da
 *
 * @returns	char count
 */
int count_chars(da_type* da);

int main(void) {
	da_type* lines = da_create(sizeof(char*));
	size_t x = 0;
	size_t y = 0;

	da_set_destructor(lines, &free);

	read_lines(filename, lines);
	da_print(lines, &string_printer_conf);

	find_char(lines, cursor_up, &x, &y);
	printf("Start index (from top-left) == %lu, %lu\n\n", x, y);

	while (advance_cursor(lines, &x, &y) == 0) {
		printf("--------------------------------------------------\n");
		da_print(lines, &string_printer_conf);
	}

	printf("--------------------------------------------------\n");
	da_print(lines, &string_printer_conf);
	printf("Finish index (from top-left) == %lu, %lu\n\n", x, y);
	printf("Unique Tiles Visited -> %i\n", count_chars(lines));

	da_destroy(lines);
	return 0;
}

void string_printer(da_type* da, size_t index) {
	printf("%s", *(char**)da_at(da, index));
}

void find_char(da_type* da, char c, size_t* x, size_t* y) {
	size_t row = 0;
	size_t col = 0;

	for (row = 0; row < da_size(da); ++row) {
		char* line = *(char**)da_at(da, row);

		for (col = 0; col < strlen(line); ++col) {
			if (line[col] == c) {
				*y = row;
				*x = col;
				return;
			}
		}
	}
}

#define check_char(da, x, y, c) (*(char**)da_at(da, y))[x] == c
#define set_char(da, x, y, c) ((*(char**)da_at(da, y))[x] = c)
#define check_and_move(da, axis, bound, xoff, yoff, c)                        \
do {                                                                          \
	if (*axis == bound) {                                                 \
		set_char(da, *x, *y, c);                                      \
		return 1;                                                     \
	}                                                                     \
	if (check_char(da, *x + xoff, *y + yoff, wall_char)) {                \
		goto rotate;                                                  \
	}                                                                     \
	*x += xoff;                                                           \
	*y += yoff;                                                           \
	set_char(da, *x, *y, c);                                              \
} while (0)                                                                   \

int advance_cursor(da_type* da, size_t* x, size_t* y) {
	size_t max_row = da_size(da) - 1;
	size_t max_col = strlen(*(char**)da_at(da, 0)) - 1;

	char c = (*(char**)da_at(da, *y))[*x];

	switch (c) {
		case cursor_up:
			check_and_move(da, y, 0, 0, -1, cursor_up);
			break;

		case cursor_left:
			check_and_move(da, x, 0, -1, 0, cursor_left);
			break;

		case cursor_down:
			check_and_move(da, y, max_row, 0, 1, cursor_down);
			break;

		case cursor_right:
			check_and_move(da, x, max_col, 1, 0, cursor_right);
			break;

		default:
			fprintf(stderr, "Invalid char %c\n", c);
			return 2;
	}

exit:
	return 0;

rotate:
	switch (c) {
		case cursor_up:
			set_char(da, *x, *y, cursor_right);
			break;

		case cursor_left:
			set_char(da, *x, *y, cursor_up);
			break;

		case cursor_down:
			set_char(da, *x, *y, cursor_left);
			break;

		case cursor_right:
			set_char(da, *x, *y, cursor_down);
			break;
	}

	goto exit;
}

int count_chars(da_type* da) {
	int count = 0;

	size_t row = 0;
	size_t col = 0;

	for (row = 0; row < da_size(da); ++row) {
		char* line = *(char**)da_at(da, row);

		for (col = 0; col < strlen(line); ++col) {
			if (line[col] == cursor_up) {
				++count;
			}
			if (line[col] == cursor_right) {
				++count;
			}
			if (line[col] == cursor_down) {
				++count;
			}
			if (line[col] == cursor_left) {
				++count;
			}
		}
	}

	return count;
}
