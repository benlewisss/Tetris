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
        SDL_LogVerbose(SDL_LOG_CATEGORY_ERROR, "Assertion failed, raising an error...");
        FatalError(errorMessage);
    }
}

void FatalError(const char* errorMessage)
{
    const char* sdlError = SDL_GetError();
    SDL_LogCritical(SDL_LOG_CATEGORY_ERROR,"%s - %s", errorMessage, sdlError);
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
        "Fatal Error",
        sdlError && *sdlError ? sdlError : errorMessage,
        NULL);

    // Clean up SDL subsystems and quit
    SDL_LogVerbose(SDL_LOG_CATEGORY_ERROR, "Quitting SDL subsystems...");
    SDL_Quit();
    SDL_LogVerbose(SDL_LOG_CATEGORY_ERROR, "Exiting with failure...");
    exit(EXIT_FAILURE);
}
