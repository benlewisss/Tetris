#ifndef UTIL_H
#define UTIL_H

#include <stdbool.h>

/* Constants */
enum MainConfig
{
    MAX_STRING_LENGTH = 32,
};

/**
 * @brief Asserts that a given value is true, and logs an error using SDL libraries if not.
 *
 * @param value The boolean value to assert.
 * @param errorMessage The error message that is logged upon failure.
 */
void Assert(bool value, char* errorMessage);

/**
 * @brief Show an SDL_Error message, log error details, and (attempt to) gracefully shut down the program.
 *
 * @threadsafety This method should only be called from the main thread.
 *
 * @param errorMessage The error message that is logged.
 */
void FatalError(const char* errorMessage);

#endif //UTIL_H
