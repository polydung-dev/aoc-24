#ifndef DA_H_
#define DA_H_

#include <stddef.h>

#define DA_INITIAL_CAP 8
#define DA_SCALE_FACTOR 2

#define da_create(da)                                                         \
do {                                                                          \
	(da) = malloc(sizeof(*(da)));                                         \
	(da)->data = calloc(DA_INITIAL_CAP, sizeof((da)->data[0]));           \
	(da)->size = 0;                                                       \
	(da)->capacity = DA_INITIAL_CAP;                                      \
} while (0)

#define da_destroy(da)                                                        \
do {                                                                          \
	if ((da) == NULL) {                                                   \
		break;                                                        \
	}                                                                     \
	free((da)->data);                                                     \
	free((da));                                                           \
} while (0)

#define da_append(da, elem)                                                   \
do {                                                                          \
	if ((da)->size == (da)->capacity) {                                   \
		da_reserve((da), (da)->capacity * DA_SCALE_FACTOR);           \
	}                                                                     \
	++(da)->size;                                                         \
	(da)->data[(da)->size - 1] = (elem);                                  \
} while (0)

#define da_reserve(da, cap)                                                   \
do {                                                                          \
	void* tmp = realloc((da)->data, (cap) * sizeof((da)->data[0]));       \
	if (tmp == NULL) {                                                    \
		fprintf(stderr, "out of memory\n");                           \
		exit(1);                                                      \
	}                                                                     \
	(da)->data = tmp;                                                     \
	(da)->capacity = (cap);                                               \
} while (0)

#endif /* DA_H_ */
