#include "da.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char filename[] = "./data/08/test.txt";

struct DAPrinterConfig printer_config = {0};
void grid_printer(da_type* da, size_t index);

struct Pos {
	int x;
	int y;
};

/**
 * Searches the grid for all "frequencies".
 *
 * @param [in]    lines	grid of all antennae
 *
 * @returns	a dynamic array of all frequencies
 */
da_type* get_frequencies(da_type* lines);

/**
 * Isolates towers of a given frequency.
 *
 * @param [in]    lines	grid of all towers
 * @param         freq 	specific frequency to isolate
 *
 * @returns	a grid of towers matching the given frequency
 */
da_type* isolate_frequency(da_type* lines, char freq);

/**
 * Calculates the locations of all antinodes for each tower pair.
 *
 * @param [in]    lines	grid of all towers
 *
 * @returns	an array of coordinates (struct Pos)
 */
da_type* calculate_nodes(da_type* lines);

int main(void) {
	size_t i = 0;
	size_t count = 0;
	da_type* frequencies = NULL;
	da_type* lines = da_create(sizeof(char*));
	da_type* node_map = da_create(sizeof(char*));
	da_type* nodes = da_create(sizeof(struct Pos));

	da_set_destructor(node_map, &free);
	da_set_destructor(lines, &free);
	printer_config.printer = &grid_printer;

	read_lines(filename, lines);

	/* construct blank field */
	{
		size_t sz = strlen(da_get_as(lines, i, char*));
		for (i = 0; i < da_size(lines); ++i) {
			char* line = malloc(sz + 1);
			memset(line, '.', sz);
			line[sz] = '\0';
			da_append(node_map, &line);
		}
	}

	frequencies = get_frequencies(lines);

	for (i = 0; i < da_size(frequencies); ++i) {
		size_t j = 0;
		char f = da_get_as(frequencies, i, char);
		da_type* towers = isolate_frequency(lines, f);
		da_type* freq_nodes = calculate_nodes(towers);


		for (j = 0; j < da_size(freq_nodes); ++j) {
			struct Pos pair = da_get_as(freq_nodes, j, struct Pos);
			da_get_as(towers, pair.y, char*)[pair.x] = '#';
			da_get_as(node_map, pair.y, char*)[pair.x] = '#';
		}

		printf("--------------------------------------------------\n");
		printf("Freq: %c, Node Count: %lu\n", f, da_size(freq_nodes));
		printf("--------------------------------------------------\n");
		da_print(towers, &printer_config);

		da_destroy(freq_nodes);
		da_destroy(towers);
	}

	for (i = 0; i < da_size(node_map); ++i) {
		size_t j = 0;
		char* line = da_get_as(node_map, i, char*);

		for (j = 0; j < strlen(line); ++j) {
			if (line[j] == '#') {
				++count;
			}
		}
	}

	printf("--------------------------------------------------\n");
	printf("Number of anti-nodes: %lu\n", count);
	printf("--------------------------------------------------\n");
	da_print(node_map, &printer_config);

	da_destroy(node_map);
	da_destroy(nodes);
	da_destroy(frequencies);
	da_destroy(lines);

	return 0;
}

void grid_printer(da_type* da, size_t index) {
	printf("%s\n", da_get_as(da, index, char*));
}

#define contains(da, value) \
	(da_find(da, da_begin(da), da_end(da), value) != da_end(da))

da_type* get_frequencies(da_type* lines) {
	da_type* frequencies = da_create(sizeof(char));
	size_t i = 0;

	/* for each line */
	for (i = 0; i < da_size(lines); ++i) {
		size_t j = 0;
		char* line = da_get_as(lines, i, char*);

		/* for each char */
		for (j = 0; j < strlen(line); ++j) {
			char c = line[j];

			/* skip empty spaces */
			if (c == '.') {
				continue;
			}

			/* append if not already in array */
			if (!contains(frequencies, &c)) {
				da_append(frequencies, &c);
			}
		}
	}

	return frequencies;
}

da_type* isolate_frequency(da_type* lines, char freq) {
	size_t i = 0;
	da_type* grid = da_create(sizeof(char*));
	da_set_destructor(grid, &free);

	/* for each line */
	for (i = 0; i < da_size(lines); ++i) {
		size_t j = 0;
		char* line = da_get_as(lines, i, char*);
		char* new_line = malloc(strlen(line) + 1);
		new_line[strlen(line)] = 0;

		if (new_line == NULL) {
			fprintf(stderr, "out of memory\n");
			exit(1);
		}

		/* for each char */
		for (j = 0; j < strlen(line); ++j) {
			char c = line[j];

			/* empty spaces and other towers */
			if (c == '.' || c != freq) {
				new_line[j] = '.';
			}

			/* target towers */
			if (c == freq) {
				new_line[j] = freq;
			}

		}

		da_append(grid, &new_line);
	}

	return grid;
}

da_type* calculate_nodes(da_type* lines) {
	da_type* nodes  = da_create(sizeof(struct Pos));
	da_type* towers = da_create(sizeof(struct Pos));
	size_t x = 0;
	size_t y = 0;
	int max_x = strlen(da_get_as(lines, 0, char*));
	int max_y = da_size(lines);

	/* get positions of each tower */
	for (y = 0; y < da_size(lines); ++y) {
		char* line = da_get_as(lines, y, char*);
		for (x = 0; x < strlen(line); ++x) {
			char c = line[x];

			if (c != '.') {
				struct Pos p = {0};
				p.x = x;
				p.y = y;

				da_append(towers, &p);
			}
		}
	}

	/* iterate through each tower pair */
	for (x = 0; x < da_size(towers) - 1; ++x) {
		for (y = x + 1; y < da_size(towers); ++y) {
			struct Pos t1_pos = da_get_as(towers, x, struct Pos);
			struct Pos t2_pos = da_get_as(towers, y, struct Pos);
			struct Pos delta  = {0};
			struct Pos n1_pos = {0};
			struct Pos n2_pos = {0};

			delta.x  = t1_pos.x - t2_pos.x;
			delta.y  = t1_pos.y - t2_pos.y;
			n1_pos.x = t1_pos.x + delta.x;
			n1_pos.y = t1_pos.y + delta.y;
			n2_pos.x = t2_pos.x - delta.x;
			n2_pos.y = t2_pos.y - delta.y;

			if (
				n1_pos.x >= 0 && n1_pos.x < max_x &&
				n1_pos.y >= 0 && n1_pos.y < max_y
			) {
				da_append(nodes, &n1_pos);
			}

			if (
				n2_pos.x >= 0 && n2_pos.x < max_x &&
				n2_pos.y >= 0 && n2_pos.y < max_y
			) {
				da_append(nodes, &n2_pos);
			}
		}
	}


	da_destroy(towers);

	return nodes;
}
