#ifndef UTIL_H
#define UTIL_H

#include <stdbool.h>

#include "SDL3/SDL_render.h"

/* Constants */
enum MainConfig
{
	MAX_LEVEL = 20,
	MAX_STRING_LENGTH = 32,
    TEXT_CACHE_SIZE = 128,
};

typedef struct TextCacheEntry
{
    char text[MAX_STRING_LENGTH];
    SDL_Texture* texture;
    struct TextCacheEntry* next;
} TextCacheEntry;

typedef struct TextCache
{
    TextCacheEntry* buckets[TEXT_CACHE_SIZE];
} TextCache;

Uint32 Djb2Hash(char* string);

TextCache* TextCacheCreate(void);

bool TextCacheDestroy(TextCache* textCache);

SDL_Texture* TextCacheGet(TextCache* textCache, char* string);

bool TextCachePut(TextCache* textCache, char* string, SDL_Texture* texture);

bool TextCacheRemove(TextCache* textCache, char* string);

/**
 * @brief Asserts that a given value is true, and logs an error using SDL libraries if not.
 *
 * @param value The boolean value to assert.
 * @param errorMessage The error message that is logged upon failure.
 * @return True if there is an assertion (i.e. value == false), false otherwise.
 */
bool Assert(bool value, char* errorMessage);

#endif //UTIL_H
