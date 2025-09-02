#ifndef UTIL_H
#define UTIL_H

#include <stdbool.h>

/* Constants */
enum MainConfig
{
	MAX_LEVEL = 20,
	MAX_STRING_LENGTH = 32,
};

/**
 * @brief Asserts that a given value is true, and logs an error using SDL libraries if not.
 *
 * @param value The boolean value to assert.
 * @param errorMessage The error message that is logged upon failure.
 */
void Assert(bool value, char* errorMessage);

#endif //UTIL_H
