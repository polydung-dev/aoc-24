#include "da.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char filename[] = "./data/15/test.txt";

struct Grid {
	char* data;
	size_t width;
	size_t height;

	size_t x_pos;
	size_t y_pos;
};

void parse_input(struct Grid* grid, char** moves, da_type* lines);

void process_move(struct Grid* grid, char move);
void display_grid(struct Grid* grid);

size_t gps_sum(struct Grid* grid);

int main(void) {
	da_type* lines = da_create(sizeof(char*));
	struct Grid grid = {0};
	char* moves = NULL;
	char* m = NULL;
	size_t output = 0;

	da_set_destructor(lines, &free);
	read_lines(filename, lines);

	parse_input(&grid, &moves, lines);

	printf("@ %lu, %lu\n", grid.x_pos, grid.y_pos);
	printf("%s\n", moves);

	for (m = moves; *m; ++m) {
		process_move(&grid, *m);
		/* display_grid(&grid); */
		/* printf("--------------------------------------------\n"); */
	}

	output = gps_sum(&grid);

	printf("GPS sum -> %lu\n", output);

	free(grid.data);
	free(moves);
	da_destroy(lines);
	return 0;
}

void parse_input(struct Grid* grid, char** moves, da_type* lines) {
	size_t i = 0;
	size_t moves_line_count = 0;
	size_t moves_line_length = 0;
	size_t moves_offset = 0;
	char* p = NULL;

	/* find break point */
	for (i = 0; i < da_size(lines); ++i) {
		if (strcmp(da_get_as(lines, i, char*), "") == 0) {
			grid->height = i;
			break;
		}
	}

	/* extract grid */
	grid->width = strlen(da_get_as(lines, 0, char*));
	grid->data = malloc(grid->width * grid->height + 1);
	p = grid->data;

	for (i = 0; i < grid->height; ++i) {
		strcpy(p, da_get_as(lines, i, char*));
		p += grid->width;
	}
	*p = '\0'; /* null terminator */

	/* extract moves */
	moves_offset = grid->height + 1;
	moves_line_count = da_size(lines) - 1 - grid->height;
	moves_line_length = strlen(da_get_as(lines, moves_offset, char*));
	*moves = malloc(moves_line_count * moves_line_length + 1);
	p = *moves;

	for (i = 0; i < moves_line_count; ++i) {
		strcpy(p, da_get_as(lines, moves_offset + i, char*));
		p += moves_line_length;
	}
	*p = '\0'; /* null terminator */

	/* get player position */
	p = grid->data;
	for (i = 0; i < grid->width * grid->height; ++i) {
		if (p[i] == '@') {
			grid->x_pos = i % grid->width;
			grid->y_pos = i / grid->width;
			break;
		}
	}
}

char* try_move(struct Grid* grid, char move) {
	char* target = &grid->data[(grid->y_pos * grid->width) + grid->x_pos];

	switch (move) {
		case '<':
			while (*target != '#') {
				target -= 1;
				if (*target == '.') {
					return target;
				}
			}
			break;
		case '>':
			while (*target != '#') {
				target += 1;
				if (*target == '.') {
					return target;
				}
			}
			break;
		case '^':
			while (*target != '#') {
				target -= grid->width;
				if (*target == '.') {
					return target;
				}
			}
			break;
		case 'v':
			while (*target != '#') {
				target += grid->width;
				if (*target == '.') {
					return target;
				}
			}
			break;
	}

	return NULL;
}

void process_move(struct Grid* grid, char move) {
	size_t new_x = 0;
	size_t new_y = 0;
	char* player = NULL;
	char* target = NULL;
	char* space = NULL;

	switch (move) {
		case '<':
			new_y = grid->y_pos;
			new_x = grid->x_pos - 1;
			break;
		case '>':
			new_y = grid->y_pos;
			new_x = grid->x_pos + 1;
			break;
		case '^':
			new_y = grid->y_pos - 1;
			new_x = grid->x_pos;
			break;
		case 'v':
			new_y = grid->y_pos + 1;
			new_x = grid->x_pos;
			break;
	}

	player = &grid->data[(grid->y_pos * grid->width) + grid->x_pos];
	target = &grid->data[(new_y * grid->width) + new_x];

	switch (*target) {
		case '#':
			break;
		case '.':
			*target = '@';
			*player = '.';
			grid->x_pos = new_x;
			grid->y_pos = new_y;
			break;
		case 'O':
			space = try_move(grid, move);
			if (space == NULL) {
				break;
			}

			*space = 'O';
			*target = '@';
			*player = '.';
			grid->x_pos = new_x;
			grid->y_pos = new_y;
	}
}

void display_grid(struct Grid* grid) {
	size_t row;
	size_t col;

	for (row = 0; row < grid->height; ++row) {
		size_t offset = row * grid->width;
		for (col = 0; col < grid->width; ++col) {
			size_t index = col + offset;

			printf("%c", grid->data[index]);
		}
		printf("\n");
	}
}

size_t gps_sum(struct Grid* grid) {
	size_t row;
	size_t col;
	size_t sum = 0;

	for (row = 0; row < grid->height; ++row) {
		size_t offset = row * grid->width;
		for (col = 0; col < grid->width; ++col) {
			size_t index = col + offset;

			if (grid->data[index] != 'O') {
				continue;
			}

			sum += (row * 100) + col;
		}
	}

	return sum;
}
