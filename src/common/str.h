#ifndef STR_UTILS_H_
#define STR_UTILS_H_

#include "da.h"

#include <stddef.h>

/**
 * @file
 *
 * A collection of string manipulation functions.
 */

/**
 * Splits the input string by the given delimeter(s).
 *
 * @param [in]    str
 * @param [in]    delim
 *
 * @returns
 */
da_type* str_split(char* str, char* delim);

/**
 * Joins all strings with the given joining string.
 *
 * @param [in]    joiner  	string to join other strings with
 * @param [in]    strings	arrray of strings [count]
 * @param         count  	number of strings
 */
char* str_join(char* joiner, char** strings, size_t count);

/**
 * Strips all char's in `to_strip` from right side of `string`.
 *
 * @param [inout] string
 * @param [in]    to_strip
 */
void str_r_strip(char* string, char* to_strip);

#endif /* STR_UTILS_H_ */
