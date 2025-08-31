#include "util.h"

#include <stdbool.h>

#include "SDL3/SDL_error.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_messagebox.h"

Uint32 Djb2Hash(const char* string)
{
    Uint32 hash = 5381;
    char character = *string;
    while (character)
    {
        hash = ((hash << 5) + hash) + character;
        character = *string++;
    }

    return hash;
}

TextCache* TextCacheCreate(void)
{ 
    TextCache* textCache = malloc(sizeof(TextCache));
    if (!textCache) return NULL;

    for (int i = 0; i < TEXT_CACHE_SIZE; i++)
    {
        textCache->buckets[i] = NULL;
    }

    return textCache;
}

bool TextCacheDestroy(TextCache* textCache)
{
    for (int i = 0; i < TEXT_CACHE_SIZE; i++)
    {
        TextCacheEntry* textCacheEntry = textCache->buckets[i];
        while (textCacheEntry)
        {
            TextCacheEntry* next = textCacheEntry->next;

            free(textCacheEntry);

            textCacheEntry = next;
        }
    }
    free(textCache);

    return true;
}

SDL_Texture* TextCacheGet(TextCache* textCache, char* string)
{
    const Uint32 key = Djb2Hash(string) % TEXT_CACHE_SIZE;

    TextCacheEntry* currentEntry = textCache->buckets[key];

    while (currentEntry)
    {
        if (strcmp(currentEntry->text, string) == 0)
        {
            return textCache->buckets[key]->texture;
        }

        currentEntry = currentEntry->next;
    }
    return NULL;
}

bool TextCachePut(TextCache* textCache, char* string, SDL_Texture* texture)
{
    if (strlen(string) >= MAX_STRING_LENGTH) // >= Because we need a byte for null terminator
    {
        return false;
    }

    const Uint32 key = Djb2Hash(string) % TEXT_CACHE_SIZE;

    TextCacheEntry* textCacheEntry = calloc(1, sizeof(TextCacheEntry));
    if (!textCacheEntry) return false;

    memcpy(textCacheEntry->text, string, MAX_STRING_LENGTH);
    textCacheEntry->texture = texture;

    // Whenever we put a value at this key, we put whatever was already this key into the location pointed to by "next"
    textCacheEntry->next = textCache->buckets[key];
    textCache->buckets[key] = textCacheEntry;
    return true;
}

bool TextCacheRemove(TextCache* textCache, char* string)
{
    const Uint32 key = Djb2Hash(string) % TEXT_CACHE_SIZE;

    TextCacheEntry* currentEntry = textCache->buckets[key];
    TextCacheEntry* previousEntry = NULL;

    while (currentEntry)
    {
        if (strcmp(currentEntry->text, string) == 0)
        {
            if (previousEntry)
            {
                previousEntry->next = currentEntry->next;
            }
            else
            {
                textCache->buckets[key] = currentEntry->next;
            }

            free(currentEntry);

            return true;
        }

        previousEntry = currentEntry;
        currentEntry = currentEntry->next;
    }

    return false;
}

bool Assert(const bool value, char* errorMessage)
{
    if (value != true)
    {
        SDL_Log("%s: %s", errorMessage, SDL_GetError());
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, errorMessage, SDL_GetError(), NULL);
        return true;
    }
    return false;
}
