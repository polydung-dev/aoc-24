#ifndef DA_H_
#define DA_H_

#include <stddef.h>

/**
 * @file
 *
 * A DynmaicArray implementation (loosely) based on `std::vector` from C++.
 *
 * todo:
 * - element "constructor"
 * - front/back/data accessors
 * - shrink_to_fit?
 * - clear
 * - better type safety (check sizes, __typeof__)
 * - generic container base? (deque)
 * - iterators proper
 * - algorithms library (copy, find, etc.)
 */

#define DA_INITIAL_CAP 8
#define DA_SCALE_FACTOR 2

typedef enum DynamicArray_Status {
	DA_OK = 0,
	DA_OUT_OF_MEMORY,
	DA_OUT_OF_BOUNDS,
	DA_INVALID_SIZE,
	DA_BAD_RANGE
} da_status_type;

typedef struct DynamicArray da_type;

typedef void destructor_fn(void* fn);

/**
 * Sets a function pointer for a "destroctor" for each element which is called
 * by `da_destroy()`.
 *
 * If `NULL` is passed as the function pointer, the "destructor" is disabled.
 *
 * @param [inout] da	some pointer returned by `da_create()`
 * @param         fn	pointer to function, or NULL
 */
void da_set_free_fn(da_type* da, destructor_fn* fn);

/*///////////////////////////////////////////////////////////////////////////*/
/* DynamicArray                                                              */
/*///////////////////////////////////////////////////////////////////////////*/

/**
 * Allocated memory and initialised a DynamicArray.
 *
 * @param elem_size	sizeof(type) for element type
 *
 * @returns	success -> pointer to a DynmaicArray
 * @returns	failure -> NULL
 *
 * @see	`da_destroy()`
 */
da_type* da_create(size_t elem_size);

/**
 * Frees memory associated with the DynamicArray.
 *
 * Note: Takes the parameter as a `void*` so it can be passed to
 * `da_set_free_fn` for nested arrays.
 *
 * @param [inout] da	some pointer returned by `da_create()`
 *
 * @see	`da_create()`
 */
void da_destroy(void* da);

/*///////////////////////////////////////////////////////////////////////////*/
/* Element Access                                                            */
/*///////////////////////////////////////////////////////////////////////////*/

/**
 * Returns a pointer to an element in the array at the requested index.
 *
 * @param [inout] da   	some pointer returned by `da_create()`
 * @param         index	an index into the array
 *
 * @returns	success -> a pointer to an element in the array
 * @returns	failure -> NULL
 *
 * Possible Error Values
 * - `DA_OK`
 * - `DA_OUT_OF_BOUNDS`
 */
#define da_at(da, index) da_at_(__FILE__, __LINE__, da, index)
void* da_at_(const char* file, size_t line, da_type* da, size_t index);

/*///////////////////////////////////////////////////////////////////////////*/
/* Iterators                                                                 */
/*///////////////////////////////////////////////////////////////////////////*/

/**
 * Returns a pointer to the beginning of the array.
 */
void* da_begin(da_type* da);

/**
 * Returns a pointer to one-past-the-end of the the array.
 */
void* da_end(da_type* da);

/*///////////////////////////////////////////////////////////////////////////*/
/* Capacity                                                                  */
/*///////////////////////////////////////////////////////////////////////////*/

/**
 * Returns the number of elements in the array.
 *
 * @param [inout] da	some pointer returned by `da_create()`
 *
 * @returns	the number of elements in the array
 */
size_t da_size(da_type* da);

/**
 * Reserves additional memory for the array.
 *
 * @param [inout] da     	some pointer returned by `da_create()`
 * @param         new_cap	the new capacity (in elements) of the array
 *
 * Possible Error Values
 * - `DA_OK`
 * - `DA_OUT_OF_MEMORY`
 * - `DA_INVALID_SIZE`
 */
#define da_reserve(da, new_cap) da_reserve_(__FILE__, __LINE__, da, new_cap)
da_status_type da_reserve_(
	const char* file, size_t line, da_type* da, size_t new_cap
);

/*///////////////////////////////////////////////////////////////////////////*/
/* Modifiers                                                                 */
/*///////////////////////////////////////////////////////////////////////////*/

/**
 * Append an element to the array.
 *
 * Data is copied over from the memory location pointed to.
 *
 * @param [inout] da   	some pointer returned by `da_create()`
 * @param [in]    value	a pointer the value of the element to append
 *
 * Possible Error Values
 * - `DA_OK`
 * - `DA_OUT_OF_MEMORY`
 */
#define da_append(da, value) da_append_(__FILE__, __LINE__, da, value)
void da_append_(const char* file, size_t line, da_type* da, void* value);

/**
 * Inserts an element into the array before the given position.
 *
 * Data is copied over from the memory location pointed to.
 *
 * @param [inout] da   	some pointer returned by `da_create()`
 * @param [in]    pos  	a pointer to an element within the array
 * @param [in]    value	a pointer the value of the element to append
 *
 * @returns	success -> pointer to inserted element
 * @returns	failure -> NULL
 *
 * Possible Error Values
 * - `DA_OK`
 * - `DA_OUT_OF_BOUNDS`
 * - `DA_OUT_OF_MEMORY`
 */
#define da_insert(da, pos, value) da_insert_(__FILE__, __LINE__, da, pos, value)
void* da_insert_(
	const char* file, size_t line,
	da_type* da, void* pos, void* value
);

/**
 * Erases an element from the array.
 *
 * Note: the array is not resized.
 *
 * @param [inout] da   	some pointer returned by `da_create()`
 * @param [in]    pos  	a pointer to an element within the array
 *
 * @returns	success -> pointer to the element after the one erased
 * @returns	failure -> NULL
 *
 * Possible Error Values
 * - `DA_OK`
 * - `DA_OUT_OF_BOUNDS`
 */
#define da_erase(da, pos) da_erase_(__FILE__, __LINE__, da, pos)
void* da_erase_(const char* file, size_t line, da_type* da, void* pos);

/*///////////////////////////////////////////////////////////////////////////*/
/* Utility                                                                   */
/*///////////////////////////////////////////////////////////////////////////*/

/**
 * Function to print a single element of an array.
 */
typedef void da_printer_fn(da_type* da, size_t index);

/**
 * Array printer configuration.
 *
 * Any of the strings may be NULL.
 */
struct DAPrinterConfig {
	da_printer_fn* printer;
	char* open;   /* prefix for array  e.g. `[` */
	char* sep;    /* element separator e.g. `, ` */
	char* close;  /* suffix for array  e.g. `]` */
	char* prefix; /* element prefix*/
};

/**
 * Prints an array.
 */
void da_print(da_type* da, struct DAPrinterConfig* config);

/**
 * Finds the first element in the array with the given value within the
 * given range (exclusive of `last`).
 *
 * @returns	success -> pointer to first found element
 * @returns	not found -> `last`
 * @returns	failure -> NULL
 *
 * Possible Error Values
 * - `DA_OK`
 * - `DA_BAD_RANGE`
 */
#define da_find(da, first, last, value) \
	da_find_(__FILE__, __LINE__, da, first, last, value)
void* da_find_(
	const char* file, size_t line,
	da_type* da, void* first, void* last, void* value
);

#endif /* DA_H_ */
