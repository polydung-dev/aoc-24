#include "da.h"
#include "str.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

size_t count_begin = 0;
size_t count_end = 100;

int save_frames = 0;

const char filename[] = "./data/14/test.txt";

struct Robot {
	int px;
	int py;
	int vx;
	int vy;
};

da_type* parse_input(da_type* lines);
void advance_robots(da_type* robots, int max_x, int max_y);

void fill_grid(
	da_type* grid, da_type* robots, size_t grid_width, size_t grid_height
);
void print_grid(da_type* grid, size_t grid_width, size_t grid_height);

int save_to_pbm(const char* path, int* data, size_t width, size_t height);

int main(void) {
	size_t i = 0;
	size_t x = 0;
	size_t y = 0;
	da_type* lines = da_create(sizeof(char*));
	da_type* robots = NULL;
	da_type* grid = da_create(sizeof(int));
	size_t grid_width = 0;
	size_t grid_height = 0;
	char* path = NULL;

	int ul_safety = 0;
	int ur_safety = 0;
	int ll_safety = 0;
	int lr_safety = 0;
	int total_safety = 0;

	da_set_destructor(lines, &free);
	read_lines(filename, lines);

	robots = parse_input(lines);
	da_destroy(lines);

	/* assuming that the grid size is based on the robot positions */
	for (i = 0; i < da_size(robots); ++i) {
		struct Robot* r = da_at(robots, i);

		if (r->px > (int)grid_width) {
			grid_width = r->px;
		}

		if (r->py > (int)grid_height) {
			grid_height = r->py;
		}
	}

	/* index/size off-by-one */
	++grid_width;
	++grid_height;

	printf(
		"Grid size: width = %lu, height = %lu\n",
		grid_width, grid_height
	);

	printf("----------------------------------------------------------\n");
	printf("Initial Layout\n");
	fill_grid(grid, robots, grid_width, grid_height);
	print_grid(grid, grid_width, grid_height);

	path = malloc(256);
	if (path == NULL) {
		fprintf(stderr, "filepath buffer: out of memory\n");
		exit(1);
	}

	for (i = count_begin; i < count_end; ++i) {
		advance_robots(robots, grid_width, grid_height);

		if (!save_frames) {
			continue;
		}

		sprintf(path, "images/%06lu.pbm", i + 1);

		fill_grid(grid, robots, grid_width, grid_height);
		save_to_pbm(path, da_data(grid), grid_width, grid_height);
	}

	free(path);

	printf("----------------------------------------------------------\n");
	printf("Final Layout\n");

	fill_grid(grid, robots, grid_width, grid_height);
	print_grid(grid, grid_width, grid_height);

	printf("----------------------------------------------------------\n");
	for (y = 0; y < (grid_height / 2); ++y) {
		for (x = 0; x < (grid_width / 2); ++x) {
			size_t offset = y * grid_width;
			size_t index = x + offset;
			int n = da_get_as(grid, index, int);

			ul_safety += n;
		}
	}
	printf("Upper Left  safety factor : %i\n", ul_safety);

	for (y = 0; y < (grid_height / 2); ++y) {
		for (x = (grid_width / 2) + 1; x < grid_width; ++x) {
			size_t offset = y * grid_width;
			size_t index = x + offset;
			int n = da_get_as(grid, index, int);

			ur_safety += n;
		}
	}
	printf("Upper Right safety factor : %i\n", ur_safety);

	for (y = (grid_height / 2) + 1; y < grid_height; ++y) {
		for (x = 0; x < (grid_width / 2); ++x) {
			size_t offset = y * grid_width;
			size_t index = x + offset;
			int n = da_get_as(grid, index, int);

			ll_safety += n;
		}
	}
	printf("Lower Left  safety factor : %i\n", ll_safety);

	for (y = (grid_height / 2) + 1; y < grid_height; ++y) {
		for (x = (grid_width / 2) + 1; x < grid_width; ++x) {
			size_t offset = y * grid_width;
			size_t index = x + offset;
			int n = da_get_as(grid, index, int);

			lr_safety += n;
		}
	}
	printf("Lower Right safety factor : %i\n", lr_safety);

	total_safety = ul_safety * ur_safety * ll_safety * lr_safety;
	printf("Total       safety factor : %i\n", total_safety);

	da_destroy(grid);
	da_destroy(robots);
	return 0;
}

da_type* parse_input(da_type* lines) {
	size_t i = 0;
	da_type* robots = da_create(sizeof(struct Robot));

	for (i = 0; i < da_size(lines); ++i) {
		char* line = da_get_as(lines, i, char*);

		struct Robot r = {0};
		da_type* d = str_split(line, " ");
		char* p_str = da_get_as(d, 0, char*);
		char* v_str = da_get_as(d, 1, char*);

		r.px = atoi(strtok(p_str + 2, ","));
		r.py = atoi(strtok(NULL, ","));
		r.vx = atoi(strtok(v_str + 2, ","));
		r.vy = atoi(strtok(NULL, ","));

		da_destroy(d);

		da_append(robots, &r);
	}

	return robots;
}

void advance_robots(da_type* robots, int max_x, int max_y) {
	size_t i = 0;

	for (i = 0; i < da_size(robots); ++i) {
		struct Robot* r = da_at(robots, i);

		r->px += r->vx;
		if (r->px < 0) {
			r->px += max_x;
		}
		if (r->px >= max_x) {
			r->px -= max_x;
		}

		r->py += r->vy;

		if (r->py < 0) {
			r->py += max_y;
		}
		if (r->py >= max_y) {
			r->py -= max_y;
		}
	}
}

void fill_grid(
	da_type* grid, da_type* robots, size_t grid_width, size_t grid_height
) {
	size_t i = 0;
	int x = 0;

	da_assign(grid, grid_width * grid_height, &x);

	for (i = 0; i < da_size(robots); ++i) {
		struct Robot* r = da_at(robots, i);
		int* n = da_at(grid, r->px + (r->py * grid_width));
		++(*n);
	}
}

void print_grid(da_type* grid, size_t grid_width, size_t grid_height) {
	size_t x = 0;
	size_t y = 0;

	for (y = 0; y < grid_height; ++y) {
		size_t offset = y * grid_width;
		for (x = 0; x < grid_width; ++x) {
			size_t index = x + offset;
			int n = da_get_as(grid, index, int);

			if (n == 0) {
				printf(".");
			} else {
				printf("%i", n);
			}
		}
		printf("\n");
	}
}

int save_to_pbm(const char* path, int* data, size_t width, size_t height) {
	FILE* fp;
	char* header;
	size_t i = 0;
	size_t pixel_count = width * height;

	fp = fopen(path, "w");
	if (fp == NULL) {
		perror("save_to_pbm: writing pbm");
		return 1;
	}

	header = calloc(32, 1);
	if (header == NULL) {
		perror("save_to_pbm: allocating header buffer");
		fclose(fp);
		return 1;
	}

	sprintf(header, "P1\n%lu %lu\n", width, height);
	fwrite(header, 1, strlen(header), fp);
	free(header);

	for(i = 0; i < pixel_count; ++i) {
		int n = data[i];

		/* netpbm has colour inverted, 1 == black, 0 == white*/
		if (n == 0) {
			fwrite("1", 1, 1, fp);
		} else {
			fwrite("0", 1, 1, fp);
		}

		if ((i + 1) % width == 0) {
			fwrite("\n", 1, 1, fp);
		}
	}

	fclose(fp);

	return 0;
}
