#include "da.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct DynamicArray {
	void*  data;      /* pointer to data storage */
	size_t size;      /* number of elements */
	size_t capacity;  /* maximum number of elements alloceted */
	size_t elem_size; /* byte size of element data type */

	/* element "destructor" */
	destructor_fn* destructor;

	/* for error reporting */
	da_status_type status;
	const char*    file;
	size_t         line;
};

void da_set_destructor(da_type* da, destructor_fn* fn) {
	da->destructor = fn;
}

/*///////////////////////////////////////////////////////////////////////////*/
/* Errors                                                                    */
/*///////////////////////////////////////////////////////////////////////////*/

char* da_strerror(da_status_type status) {
	switch (status) {
		case DA_OK:            return "ok";
		case DA_OUT_OF_MEMORY: return "out of memory";
		case DA_OUT_OF_BOUNDS: return "out of bounds";
		case DA_INVALID_SIZE:  return "invalid size";
		case DA_BAD_RANGE:     return "bad range";
		default:               return "unkown error";
	}
}

void da_perror(da_type* da, const char* s) {
	if (s != NULL) {
		printf("%s: ", s);
	}

	printf("%s @ %s:%lu\n", da_strerror(da->status), da->file, da->line);
}

/*///////////////////////////////////////////////////////////////////////////*/
/* DynamicArray                                                              */
/*///////////////////////////////////////////////////////////////////////////*/

da_type* da_create(size_t elem_size) {
	da_type* da = malloc(sizeof(da_type));
	if (da == NULL) {
		/* ENOMEM */
		perror("da create");
		return NULL;
	}

	da->data = calloc(DA_INITIAL_CAP, elem_size);
	if (da->data == NULL) {
		/* ENOMEM */
		perror("da create");
		return NULL;
	}

	da->size      = 0;
	da->capacity  = DA_INITIAL_CAP;
	da->elem_size = elem_size;
	da->destructor   = NULL;

	da->status = DA_OK;
	da->file   = NULL;
	da->line   = 0;

	return da;
}

void da_destroy(void* tmp) {
	da_type* da = tmp;
	size_t i = 0;
	if (da == NULL) {
		return;
	}

	if (da->destructor != NULL) {
		for (i = 0; i < da->size; ++i) {
			void* elem = (char*)da->data + (i * da->elem_size);
			da->destructor(*(void**)elem);
		}
	}

	free(da->data);
	free(da);
}

void da_assign_(
	const char* file, size_t line, da_type* da, size_t count, void* value
) {
	size_t i = 0;

	da->status = DA_OK;
	da->file   = file;
	da->line   = line;

	da_clear(da);

	if (count > da->capacity) {
		da_reserve(da, count);

		/* pass error back up to caller */
		if (da->status != DA_OK) {
			return;
		}
	}

	for (i = 0; i < count; ++i) {
		da_append(da, value);
	}
}

/*///////////////////////////////////////////////////////////////////////////*/
/* Element Access                                                            */
/*///////////////////////////////////////////////////////////////////////////*/

void* da_at_(const char* file, size_t line, da_type* da, size_t index) {
	da->status = DA_OK;
	da->file   = file;
	da->line   = line;

	if (index >= da->size) {
		da->status = DA_OUT_OF_BOUNDS;
		return NULL;
	}

	return (char*)da->data + (index * da->elem_size);
}

void* da_data(da_type* da) {
	if (da->size == 0) {
		return NULL;
	}

	return da->data;
}

/*///////////////////////////////////////////////////////////////////////////*/
/* "Iterators"                                                               */
/*///////////////////////////////////////////////////////////////////////////*/

void* da_begin(da_type* da) {
	return da->data;
}

void* da_end(da_type* da) {
	return (char*)da->data + (da->size * da->elem_size);
}

/*///////////////////////////////////////////////////////////////////////////*/
/* Capacity                                                                  */
/*///////////////////////////////////////////////////////////////////////////*/

size_t da_size(da_type* da) {
	return da->size;
}

da_status_type da_reserve_(
	const char* file, size_t line, da_type* da, size_t new_cap
) {
	void* tmp = NULL;

	da->status = DA_OK;
	da->file   = file;
	da->line   = line;

	if (new_cap <= da->capacity) {
		da->status = DA_INVALID_SIZE;
		return da->status;
	}

	tmp = realloc(da->data, new_cap * da->elem_size);
	if (tmp == NULL) {
		da->status = DA_OUT_OF_MEMORY;
		return da->status;
	}

	da->data = tmp;
	da->capacity = new_cap;

	return da->status;
}

/*///////////////////////////////////////////////////////////////////////////*/
/* Modifiers                                                                 */
/*///////////////////////////////////////////////////////////////////////////*/

void da_clear(da_type* da) {
	if (da->destructor == NULL) {
		memset(da->data, 0, da->size * da->elem_size);
	} else {
		void* it = NULL;
		for (it = da_begin(da); it != da_end(da); /**/ ) {
			da->destructor(it);
			it = (char*)it + da->elem_size;
		}
	}

	da->size = 0;
	return;
}

ptrdiff_t da_append_(const char* file, size_t line, da_type* da, void* value) {
	char* dst = NULL;

	/* keep track of offset */
	char* old = da->data;
	ptrdiff_t offset = 0;

	da->status = DA_OK;
	da->file   = file;
	da->line   = line;

	if (da->size == da->capacity) {
		da_reserve_(file, line, da, da->capacity * DA_SCALE_FACTOR);

		/* pass error back up to caller */
		if (da->status != DA_OK) {
			return 0;
		}

		offset = (char*)da->data - old;
	}

	dst = da_end(da);
	memcpy(dst, value, da->elem_size);
	++da->size;

	return offset;
}

void* da_insert_(
	const char* file, size_t line, da_type* da, void* pos, void* value
) {
	char* dst = NULL;
	char* src = pos;
	/* need to update src if rellocation occurs */
	char* old = da->data;
	ptrdiff_t diff = 0;

	da->status = DA_OK;
	da->file   = file;
	da->line   = line;

	if (da->size == da->capacity) {
		da_reserve_(file, line, da, da->capacity * DA_SCALE_FACTOR);

		/* pass error back up to caller */
		if (da->status != DA_OK) {
			return NULL;
		}

		diff = (char*)da->data - old;
	}

	/* update pointer to new value */
	if (diff != 0) {
		src += diff;
	}

	/* move elements up */
	dst = src + da->elem_size;
	diff = (char*)da_end(da) - src;
	memmove(dst, src, diff);

	/* insert new element */
	memcpy(dst, value, da->elem_size);
	++da->size;

	return src;
}

void* da_erase_(const char* file, size_t line, da_type* da, void* pos) {
	char* dst = NULL;
	char* src = pos;
	size_t n = 0;

	da->status = DA_OK;
	da->file   = file;
	da->line   = line;

	/* move elements down */
	dst = pos;
	src = (char*)pos + da->elem_size;
	n = (char*)da_end(da) - src;
	memmove(dst, src, n);

	/* erase last element */
	if (da->destructor != NULL) {
		da->destructor((char*)da_end(da) - da->elem_size);
	} else {
		memset((char*)da_end(da) - da->elem_size, 0, da->elem_size);
	}
	--da->size;

	return dst;
}

/*///////////////////////////////////////////////////////////////////////////*/
/* Utility                                                                   */
/*///////////////////////////////////////////////////////////////////////////*/

void da_print(da_type* da, struct DAPrinterConfig* config) {
	size_t i = 0;

	if (config->open != NULL) {
		printf("%s", config->open);
	}

	for (i = 0; i < da->size; ++i) {
		if (config->prefix != NULL) {
			printf("%s", config->prefix);
		}

		config->printer(da, i);

		if (config->sep != NULL) {
			if (i + 1 < da->size) {
				printf("%s", config->sep);
			}
		}
	}

	if (config->close != NULL) {
		printf("%s", config->close);
	}
}

void* da_find_(
	const char* file, size_t line,
	da_type* da, void* first, void* last, void* value
) {
	da->status = DA_OK;
	da->file   = file;
	da->line   = line;

	if (first > last) {
		da->status = DA_BAD_RANGE;
		return NULL;
	}

	if (first < da_begin(da) || last > da_end(da)) {
		da->status = DA_OUT_OF_BOUNDS;
		return NULL;
	}

	for (; first < last; first = ((char*)first + da->elem_size)) {
		if (memcmp(first, value, da->elem_size) == 0) {
			return first;
		}
	}

	return last;
}
