#include "util.h"

#include <stdbool.h>
#include <stdlib.h>

#include "SDL3/SDL_error.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_messagebox.h"

void Assert(const bool value, const char* errorMessage)
{
    if (value != true)
    {
        FatalError(errorMessage);
    }
}

void FatalError(const char* errorMessage)
{
    const char* sdlError = SDL_GetError();

    SDL_Log("FATAL ERROR: %s: %s", errorMessage, sdlError);
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
        "Fatal Error",
        sdlError && *sdlError ? sdlError : errorMessage,
        NULL);

    // Clean up SDL subsystems and quit
    SDL_Quit();
    exit(EXIT_FAILURE);
}
