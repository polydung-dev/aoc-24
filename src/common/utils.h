#ifndef IO_UTILS_H_
#define IO_UTILS_H_

#include "da.h"

/**
 * The maximum size of a single line (in bytes)
 */
#define READ_BUFFER_SIZE 1024

/**
 * Reads a file and splits each line into a dynamic array.
 *
 * @param [in]    path
 * @param [inout] lines [elem_type -> char*]
 */
void read_lines(const char* path, da_type* lines);

#endif /* IO_UTILS_H_ */
