#include "util.h"

#include <stdbool.h>

#include "SDL3/SDL_error.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_messagebox.h"

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
