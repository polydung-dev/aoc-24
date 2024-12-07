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

void da_set_free_fn(da_type* da, destructor_fn* fn) {
	da->destructor = fn;
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

void da_append_(const char* file, size_t line, da_type* da, void* value) {
	char* dst = NULL;

	da->status = DA_OK;
	da->file   = file;
	da->line   = line;

	if (da->size == da->capacity) {
		da_reserve_(file, line, da, da->capacity * DA_SCALE_FACTOR);

		/* pass error back up to caller */
		if (da->status != DA_OK) {
			return;
		}
	}

	dst = da_end(da);
	memcpy(dst, value, da->elem_size);
	++da->size;
}

/*///////////////////////////////////////////////////////////////////////////*/
/* Utility                                                                   */
/*///////////////////////////////////////////////////////////////////////////*/

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
