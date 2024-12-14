#include "da.h"
#include "str.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Pair {
	unsigned int x;
	unsigned int y;
};

struct Grid {
	char* data;
	size_t width;
	size_t height;
};

const char filename[] = "./data/10/test.txt";

da_type* get_head_coords(struct Grid* grid);

/**
 * @param	heads	(modified in place)
 */
void advance_heads(struct Grid* grid, da_type* heads);

/**
 * @param	heads	(modified in place)
 */
void sort_and_remove_duplicates(da_type* heads);

int main(void) {
	size_t i = 0;
	size_t count = 0;
	da_type* lines = da_create(sizeof(char*));
	da_type* heads = NULL;
	da_type* current_head = NULL;
	struct Grid grid = {0};

	da_set_destructor(lines, &free);
	read_lines(filename, lines);

	grid.data = str_join("", (char**)da_data(lines), da_size(lines));
	grid.width = strlen(da_get_as(lines, 0, char*));
	grid.height = da_size(lines);

	for (i = 0; i < da_size(lines); ++i) {
		printf("%s\n", da_get_as(lines, i, char*));
	}

	heads = get_head_coords(&grid);

	printf("----------------------------------------------------------\n");
	printf("%lu trail heads found!\n", da_size(heads));
	printf("----------------------------------------------------------\n");

	current_head = da_create(sizeof(struct Pair));

	for (i = 0; i < da_size(heads); ++i) {
		size_t j = 0;
		struct Pair* p = da_at(heads, i);
		printf("Head %3lu : x = %2i, y = %2i : ", i, p->x, p->y);
		da_clear(current_head);
		da_append(current_head, da_at(heads, i));

		for (j = 0; j < 9; ++j) {
			advance_heads(&grid, current_head);
			sort_and_remove_duplicates(current_head);
		}

		count += da_size(current_head);
		printf("%lu summits reached!\n", da_size(current_head));
		da_clear(current_head);
	}

	printf("Total score: %lu\n", count);

	da_destroy(current_head);

	free(grid.data);
	da_destroy(heads);
	da_destroy(lines);
	return 0;
}

da_type* get_head_coords(struct Grid* grid) {
	size_t row = 0;
	size_t col = 0;
	da_type* heads = da_create(sizeof(struct Pair));

	for (row = 0; row < grid->height; ++row) {
		size_t offset = row * grid->width;
		for (col = 0; col < grid->width; ++col) {
			size_t index = col + offset;
			int c = grid->data[index] - '0';
			if (c == 0) {
				struct Pair p;
				p.x = col;
				p.y = row;
				da_append(heads, &p);
			}
		}
	}

	return heads;
}

#define offset_pointer(p, o) (void*)((char*)(p) + (o))

void advance_heads(struct Grid* grid, da_type* heads) {
	struct Pair* head = NULL;
	size_t offset = 0;
	size_t index = 0;
	unsigned int head_height = 0;
	size_t i = 0;
	size_t count = da_size(heads);

	if (da_size(heads) == 0) {
		return;
	}

	/* get current height */
	head = da_at(heads, 0);
	offset = head->y * grid->width;
	index  = head->x + offset;
	head_height = grid->data[index];

	head = da_begin(heads);
	for (i = 0; i < count; ++i) {
		ptrdiff_t offset = 0;
		offset = head->y * grid->width;
		index  = head->x + offset;

		/* check north */
		if (head->y > 0) {
			int target_height = grid->data[index - grid->width];
			if (target_height - head_height == 1) {
				struct Pair p;
				p.x = head->x;
				p.y = head->y - 1;

				offset = da_append(heads, &p);
				if (offset != 0) {
					head = offset_pointer(head, offset);
				}
			}
		}

		/* check east */
		if (head->x < grid->width - 1) {
			int target_height = grid->data[index + 1];
			if (target_height - head_height == 1) {
				struct Pair p;
				p.x = head->x + 1;
				p.y = head->y;

				offset = da_append(heads, &p);
				if (offset != 0) {
					head = offset_pointer(head, offset);
				}
			}
		}

		/* check south */
		if (head->y < grid->height - 1) {
			int target_height = grid->data[index + grid->width];
			if (target_height - head_height == 1) {
				struct Pair p;
				p.x = head->x;
				p.y = head->y + 1;

				offset = da_append(heads, &p);
				if (offset != 0) {
					head = offset_pointer(head, offset);
				}
			}
		}

		/* check west */
		if (head->x > 0) {
			int target_height = grid->data[index - 1];
			if (target_height - head_height == 1) {
				struct Pair p;
				p.x = head->x - 1;
				p.y = head->y;

				offset = da_append(heads, &p);
				if (offset != 0) {
					head = offset_pointer(head, offset);
				}
			}
		}

		head = da_erase(heads, head);
	}
}

int sort_fn(const void* va, const void* vb) {
	const struct Pair* a = va;
	const struct Pair* b = vb;

	if (a->x > b->x) {
		return 1;
	}

	if (a->x < b->x) {
		return -1;
	}

	if (a->y > b->y) {
		return 1;
	}

	if (a->y < b->y) {
		return -1;
	}

	return 0;
}

void sort_and_remove_duplicates(da_type* heads) {
	struct Pair* prev = da_begin(heads);
	struct Pair* it = prev + 1;

	qsort(da_data(heads), da_size(heads), sizeof(struct Pair), &sort_fn);

	while ((void*)it != da_end(heads)) {
		if (prev->x == it->x && prev->y == it->y) {
			it = da_erase(heads, it);
		} else {
			prev = it;
			++it;
		}
	}
}
