#include "da.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct DA_string {
	char** data;
	size_t size;
	size_t capacity;
};

enum Direction {
	DIR_NONE  = 0,
	DIR_NORTH = 1,
	DIR_EAST  = 2,
	DIR_SOUTH = 4,
	DIR_WEST  = 8
};

/**
 * Convert a direction into a string, string must be free'd.
 */
char* dir_string(enum Direction dir);

void read_lines(const char* path, struct DA_string* lines);

/**
 * Strips all char's in `to_strip` from right side of `string`.
 *
 * @param [inout] string
 * @param [in]    to_strip
 */
void r_strip(char* string, char* to_strip);

/**
 * Counts the number of "XMAS" strings.
 *
 * @param [in]    search_grid [num_cols * num_rows]
 * @param         num_cols
 * @param         num_rows
 * @param [inout] display_grid [num_cols * num_rows]
 *
 * @return word count
 */
size_t find_xmas(
	char* search_grid, size_t num_cols, size_t num_rows,
	enum Direction direction, char* display_grid
);

int main(void) {
	struct DA_string* lines = NULL;
	size_t i = 0;
	size_t num_cols = 0;
	size_t num_rows = 0;
	size_t grid_size = 0;
	size_t count = 0;
	char* grid = NULL;
	char* output = NULL;

	da_create(lines);
	read_lines("./data/test.txt", lines);

	/* assuming all lines are equal in length! */
	num_cols = strlen(lines->data[0]);
	num_rows = lines->size;

	printf("%lu x %lu grid\n", num_cols, num_rows);
	grid_size = num_cols * num_rows;

	/* convert lines into a flat array */
	output = malloc(grid_size);
	memset(output, '.', grid_size);

	grid = malloc(grid_size);
	for (i = 0; i < lines->size; ++i) {
		memcpy(grid + (i * num_cols), lines->data[i], num_cols);
	}

	/* search for words */
	count += find_xmas(grid, num_cols, num_rows, DIR_NORTH, output);
	count += find_xmas(grid, num_cols, num_rows, DIR_EAST,  output);
	count += find_xmas(grid, num_cols, num_rows, DIR_SOUTH, output);
	count += find_xmas(grid, num_cols, num_rows, DIR_WEST,  output);
	count += find_xmas(grid, num_cols, num_rows, DIR_NORTH | DIR_EAST, output);
	count += find_xmas(grid, num_cols, num_rows, DIR_NORTH | DIR_WEST, output);
	count += find_xmas(grid, num_cols, num_rows, DIR_SOUTH | DIR_EAST, output);
	count += find_xmas(grid, num_cols, num_rows, DIR_SOUTH | DIR_WEST, output);

	for (i = 0; i < num_rows; ++i) {
		printf("%.*s\n", (int)num_cols, output + (i * num_cols));
	}

	printf("Word count == %lu\n", count);


	for (i = 0; i < lines->size; ++i) {
		free(lines->data[i]);
	}

	free(grid);
	free(output);
	da_destroy(lines);
	return 0;
}

char* dir_string(enum Direction dir) {
	char* s = calloc(1, sizeof("north south east west "));
	int offset = 0;

	if (dir & DIR_NORTH) {
		strcpy(s + offset, "North ");
		offset += 6;
	}
	if (dir & DIR_SOUTH) {
		strcpy(s + offset, "South ");
		offset += 6;
	}
	if (dir & DIR_EAST) {
		strcpy(s + offset, "East ");
		offset += 5;
	}
	if (dir & DIR_WEST) {
		strcpy(s + offset, "West ");
		offset += 5;
	}

	if (dir == DIR_NONE) {
		strcpy(s, "(none)");
	}

	return s;
}

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

size_t find_xmas(
	char* search_grid, size_t num_cols, size_t num_rows,
	enum Direction direction, char* display_grid
) {
	size_t x = 0;
	size_t y = 0;
	size_t count = 0;

	/**
	 * These variables control the offsets to allow this to be a single
	 * function. I'm not sure if this is actually better than eight
	 * separate functions, one for each cardinal direction, but it is
	 * certainly something.
	 */
	size_t h_sa = (direction & DIR_NORTH) ? 3 : 0;
	size_t h_ea = (direction & DIR_SOUTH) ? 3 : 0;
	size_t v_sa = (direction & DIR_WEST) ? 3 : 0;
	size_t v_ea = (direction & DIR_EAST) ? 3 : 0;
	size_t v_off = 0;
	size_t h_off = 0;

	if (direction & DIR_NORTH || direction & DIR_SOUTH) {
		v_off = num_cols * ((direction & DIR_NORTH) ? -1 : 1);
	}
	if (direction & DIR_EAST || direction & DIR_WEST) {
		h_off = (direction & DIR_EAST) ? 1 : -1;
	}

	/* discard illegal directions */
	if (
		((direction & (DIR_NORTH | DIR_SOUTH)) == (DIR_NORTH | DIR_SOUTH)) ||
		((direction & (DIR_EAST  | DIR_WEST )) == (DIR_EAST  | DIR_WEST )) ||
		(direction == DIR_NONE)
	) {
		char* s = dir_string(direction);
		fprintf(stderr, "Illegal direction %s\n", s);
		free(s);
		exit(1);
	}

	for (y = h_sa; y < num_rows - h_ea; ++y) {
		size_t offset = y * num_cols;
		for (x = v_sa; x < num_cols - v_ea; ++x) {
			size_t index = x + offset;

			/* brute force */
			if (
				(search_grid[index + (0 * h_off) + (0 * v_off)] != 'X') ||
				(search_grid[index + (1 * h_off) + (1 * v_off)] != 'M') ||
				(search_grid[index + (2 * h_off) + (2 * v_off)] != 'A') ||
				(search_grid[index + (3 * h_off) + (3 * v_off)] != 'S')
			) {
				continue;
			}

			/* match found, insert into output grid */
			display_grid[index + (0 * h_off) + (0 * v_off)] = 'X';
			display_grid[index + (1 * h_off) + (1 * v_off)] = 'M';
			display_grid[index + (2 * h_off) + (2 * v_off)] = 'A';
			display_grid[index + (3 * h_off) + (3 * v_off)] = 'S';
			++count;
		}
	}
	return count;
}
