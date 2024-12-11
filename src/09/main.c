#include "da.h"
#define READ_BUFFER_SIZE (1024*1024)
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char filename[] = "./data/09/test.txt";

enum BlockType {
	BLOCK_EMPTY = 0,
	BLOCK_FILE
};

static int no_file = -1;

da_type* get_layout(char* map);
void compress_filesystem(da_type* layout);
size_t calculate_checksum(da_type* layout);

int main(void) {
	size_t i = 0;
	size_t checksum = 0;
	da_type* lines = da_create(sizeof(char*));
	da_type* layout = NULL;
	char* line = NULL;

	da_set_destructor(lines, &free);
	read_lines(filename, lines);

	/* All input is on a single line */
	line = da_get_as(lines, 0, char*);

	printf("Disk Map\n");
	printf("========\n");
	printf("%s\n", line);
	printf("\n");

	layout = get_layout(line);
	printf("Filesystem Layout\n");
	printf("=================\n");
	printf("|");
	for (i = 0; i < da_size(layout); ++i) {
		int x = da_get_as(layout, i, int);
		if (x == -1) {
			printf(".|");
		} else {
			printf("%i|", x);
		}
	}
	printf("\n\n");

	/* modifies in place */
	compress_filesystem(layout);
	printf("Compressed Filesystem Layout\n");
	printf("============================\n");
	printf("|");
	for (i = 0; i < da_size(layout); ++i) {
		int x = da_get_as(layout, i, int);
		if (x == -1) {
			printf(".|");
		} else {
			printf("%i|", x);
		}
	}
	printf("\n\n");

	checksum = calculate_checksum(layout);
	printf("Filesystem Checksum\n");
	printf("===================\n");
	printf("%lu\n", checksum);

	da_destroy(layout);
	da_destroy(lines);

	return 0;
}

da_type* get_layout(char* map) {
	size_t i = 0;
	int block_type = BLOCK_FILE;
	int file_id = 0;
	da_type* layout = da_create(sizeof(int));

	for (i = 0; i < strlen(map); ++i) {
		int j = 0;
		int n = map[i] - '0';

		switch (block_type) {
			case BLOCK_EMPTY:
				for (j = 0; j < n; ++j) {
					da_append(layout, &no_file);
				}
				block_type = BLOCK_FILE;
				break;

			case BLOCK_FILE:
				for (j = 0; j < n; ++j) {
					da_append(layout, &file_id);
				}
				block_type = BLOCK_EMPTY;
				++file_id;
				break;
		}
	}

	return layout;
}

void compress_filesystem(da_type* layout) {
	int* ptr_begin = da_begin(layout);
	int* ptr_end = (int*)da_end(layout) - 1;

	while (ptr_begin < ptr_end) {
		/* get last non-empty block */
		while (*ptr_end == no_file) {
			--ptr_end;
		}

		/* get first empty block */
		while (*ptr_begin != no_file) {
			++ptr_begin;
		}

		if (ptr_begin >= ptr_end) {
			break;
		}

		/* copy block */
		*ptr_begin = *ptr_end;

		/* erase last block */
		*ptr_end = no_file;

		/*
		printf("|");
		for (i = 0; i < da_size(layout); ++i) {
			int x = da_get_as(layout, i, int);
			if (x == -1) {
				printf(".|");
			} else {
				printf("%i|", x);
			}
		}
		printf("\n");
		*/
	}
}

size_t calculate_checksum(da_type* layout) {
	size_t checksum = 0;
	size_t i = 0;

	for (i = 0; i < da_size(layout); ++i) {
		int x = da_get_as(layout, i, int);
		if (x == no_file) {
			break;
		}
		checksum += (i * x);
	}

	return checksum;
}
