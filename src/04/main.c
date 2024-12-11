#include "da.h"
#include "utils.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char filename[] = "./data/04/test.txt";

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
/**
 * Counts the number of "X-MAS" strings.
 *
 * @param [in]    search_grid [num_cols * num_rows]
 * @param         num_cols
 * @param         num_rows
 * @param [inout] display_grid [num_cols * num_rows]
 *
 * @return word count
 */
size_t find_x_mas(
	char* search_grid, size_t num_cols, size_t num_rows, char* display_grid
);

int main(void) {
	da_type* lines = da_create(sizeof(char*));
	size_t i = 0;
	size_t num_cols = 0;
	size_t num_rows = 0;
	size_t grid_size = 0;
	size_t count = 0;
	char* grid = NULL;
	char* output = NULL;

	da_set_destructor(lines, &free);
	read_lines(filename, lines);

	/* assuming all lines are equal in length! */
	num_cols = strlen(da_get_as(lines, 0, char*));
	num_rows = da_size(lines);

	printf("%lu x %lu grid\n\n", num_cols, num_rows);
	grid_size = num_cols * num_rows;

	/* convert lines into a flat array */
	output = malloc(grid_size);
	grid = malloc(grid_size);
	for (i = 0; i < da_size(lines); ++i) {
		memcpy(grid + (i * num_cols), da_get_as(lines, i, char*), num_cols);
	}


	/* part 1 */
	memset(output, '.', grid_size);
	count = 0;
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

	printf("XMAS count == %lu\n\n", count);

	/* part 2 */
	memset(output, '.', grid_size);
	count = 0;
	count += find_x_mas(grid, num_cols, num_rows, output);

	for (i = 0; i < num_rows; ++i) {
		printf("%.*s\n", (int)num_cols, output + (i * num_cols));
	}

	printf("X-MAS count == %lu\n\n", count);

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
	int v_off = 0;
	int h_off = 0;

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

size_t find_x_mas(
	char* grid, size_t num_cols, size_t num_rows, char* display
) {
	size_t x = 0;
	size_t y = 0;
	size_t count = 0;

	for (y = 1; y < num_rows - 1; ++y) {
		size_t offset = y * num_cols;
		for (x = 1; x < num_cols - 1; ++x) {
			size_t index = x + offset;

			size_t tl = index - 1 - num_cols;
			size_t tr = index + 1 - num_cols;
			size_t bl = index - 1 + num_cols;
			size_t br = index + 1 + num_cols;

			if (grid[index] != 'A') {
				continue;
			}

			if (
				!(grid[tl] == 'M' && grid[br] == 'S') &&
				!(grid[tl] == 'S' && grid[br] == 'M')
			) {
				continue;
			}

			if (
				!(grid[bl] == 'M' && grid[tr] == 'S') &&
				!(grid[bl] == 'S' && grid[tr] == 'M')
			) {
				continue;
			}

			display[index] = grid[index];
			display[tl]    = grid[tl];
			display[tr]    = grid[tr];
			display[bl]    = grid[bl];
			display[br]    = grid[br];
			++count;
		}
	}
	return count;
}
