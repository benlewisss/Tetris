#include "util.h"

#include <assert.h>
#include <stdbool.h>

#include "SDL3/SDL_error.h"
#include "SDL3/SDL_log.h"

void Assert(const bool value, char* errorMessage)
{
    // TODO Figure out how to exit cleanly upon an error like this.
    if (value != true)
    {
        SDL_Log("%s: %s", errorMessage, SDL_GetError());
    }
}
