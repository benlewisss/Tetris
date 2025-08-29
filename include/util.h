#ifndef UTIL_H
#define UTIL_H

#include <stdbool.h>

/* Constants */
enum MainConfig
{
	ARENA_WIDTH = 10, // Default = 10
	ARENA_HEIGHT = 20, // Default = 20
	MAX_LEVEL = 20,
	MAX_STRING_LENGTH = 32,
};

/**
 * @brief Asserts that a given value is true, and logs an error using SDL libraries if not.
 * 
 * @param value The boolean value to assert.
 * @param errorMessage The error message that is logged upon failure.
 * @return True if there is an assertion (i.e. value == false), false otherwise.
 */
bool Assert(bool value, char* errorMessage);

#endif //UTIL_H
