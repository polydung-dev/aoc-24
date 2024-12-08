#ifndef IO_UTILS_H_
#define IO_UTILS_H_

#include "da2.h"

#include <stddef.h>

/**
 * Reads a file and splits each line into a dynamic array.
 *
 * @param [in]    path
 * @param [inout] lines [elem_type -> char*]
 */
void read_lines(const char* path, da_type* lines);

/**
 * @file
 *
 * A collection of string manipulation functions.
 */

/**
 * Strips all char's in `to_strip` from right side of `string`.
 *
 * @param [inout] string
 * @param [in]    to_strip
 */
void r_strip(char* string, char* to_strip);

/**
 * Joins all strings with the given joining string.
 *
 * @param [in]    joiner  	string to join other strings with
 * @param [in]    strings	arrray of strings [count]
 * @param         count  	number of strings
 */
char* join(char* joiner, char** strings, size_t count);

#endif /* IO_UTILS_H_ */
