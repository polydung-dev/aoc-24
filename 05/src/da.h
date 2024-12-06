#ifndef DA_H_
#define DA_H_

#include <stddef.h>

#define DA_INITIAL_CAP 8
#define DA_SCALE_FACTOR 2

#define da_print(da, fmt)                                                     \
do {                                                                          \
	size_t idx = 0;                                                       \
	printf("[");                                                          \
	for (idx = 0; idx < (da)->size; ++idx) {                              \
		printf(fmt, (da)->data[idx]);                                 \
		if (idx + 1 < (da)->size) {                                   \
			printf(", ");                                         \
		}                                                             \
	}                                                                     \
	printf("]\n");                                                        \
} while (0)

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

#define da_destroy_fn(da, fn)                                                 \
do {                                                                          \
	size_t i = 0;                                                         \
	if ((da) == NULL) {                                                   \
		break;                                                        \
	}                                                                     \
	for (i = 0; i < (da)->size; ++i) {                                    \
		fn((da)->data[i]);                                            \
	}                                                                     \
	free((da)->data);                                                     \
	free((da));                                                           \
} while (0)

#define da_append(da, elem)                                                   \
do {                                                                          \
	if ((da)->size == (da)->capacity) {                                   \
		da_resize((da), (da)->capacity * DA_SCALE_FACTOR);            \
	}                                                                     \
	++(da)->size;                                                         \
	(da)->data[(da)->size - 1] = (elem);                                  \
} while (0)

#define da_copy(dst, src)                                                     \
do {                                                                          \
	(dst) = malloc(sizeof(*(dst)));                                       \
	(dst)->data = calloc((src)->size, sizeof((dst)->data[0]));            \
	(dst)->size = (src)->size;                                            \
	(dst)->capacity = (src)->size;                                        \
	memcpy((dst)->data, (src)->data, (src)->size * sizeof(*(src)->data)); \
} while (0)

#define da_find(da, elem, idx)                                                \
do {                                                                          \
	size_t i_ = 0;                                                        \
	idx = -1;                                                             \
	for (i_ = 0; i_ < (da)->size; ++i_) {                                 \
		if ((da)->data[i_] == (elem)) {                               \
			idx = i_;                                             \
			break;                                                \
		}                                                             \
	}                                                                     \
} while (0)

#define da_erase(da, idx)                                                     \
do {                                                                          \
	void* dst = NULL;                                                     \
	void* src = NULL;                                                     \
	size_t n = 0;                                                         \
	if ((idx) < 0 || (idx) >= (da)->size) {                               \
		fprintf(stderr, "out of bounds\n");                           \
		exit(1);                                                      \
	}                                                                     \
	/* move elements down */                                              \
	if (idx < (da)->size) {                                               \
		dst = (da)->data + (idx);                                     \
		src = (da)->data + (idx) + 1;                                 \
		n = sizeof((da)->data[0]) * ((da)->size - 1 - (idx));         \
		memmove(dst, src, n);                                         \
	}                                                                     \
	/* delete last element */                                             \
	memset(&(da)->data[(da)->size - 1], 0, sizeof((da)->data[0]));        \
	--(da)->size;                                                         \
} while (0)

#define da_resize(da, cap)                                                    \
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
