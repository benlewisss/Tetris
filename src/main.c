#define SDL_MAIN_USE_CALLBACKS 1

#include <SDL3/SDL_main.h>
#include "SDL3_ttf/SDL_ttf.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "util.h"
#include "tetromino.h"
#include "game.h"
#include "graphics.h"


static const struct
{
    const char* key;
    const char* value;
} EXTENDED_METADATA[] =
{
    {SDL_PROP_APP_METADATA_CREATOR_STRING, "@benlewisss"},
    {SDL_PROP_APP_METADATA_COPYRIGHT_STRING, "MIT"},
    {SDL_PROP_APP_METADATA_TYPE_STRING, "Tetris"}
};

typedef struct
{
    GameDataContext* gameDataContext;
    GraphicsDataContext* graphicsDataContext;
    Fonts* fonts;
    Textures* textures;
    bool isRunning;
} AppState;

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
    // Setup application metadata
    if (Assert(SDL_SetAppMetadata("TETRIS", "1.0", "Tetris"), "Failed to initialise app metadata!\n")) return SDL_APP_FAILURE;

    for (size_t i = 0; i < SDL_arraysize(EXTENDED_METADATA); i++)
    {
        if (Assert(SDL_SetAppMetadataProperty(EXTENDED_METADATA[i].key, EXTENDED_METADATA[i].value), "Failed to set metadata properties!\n")) return SDL_APP_FAILURE;
    }

    // Initialise SDL & TTF 
    if (Assert(SDL_Init(SDL_INIT_VIDEO), "Failed to initialise SDL!\n")) return SDL_APP_FAILURE;
    if (Assert(TTF_Init(), "Failed to initialise TTF!\n")) return SDL_APP_FAILURE;


    GameDataContext* gameDataContext = SDL_calloc(1, sizeof(GameDataContext));
    GraphicsDataContext* graphicsDataContext = SDL_calloc(1, sizeof(GraphicsDataContext));
    Fonts* fonts = SDL_calloc(1, sizeof(Fonts));
    Textures* textures = SDL_calloc(1, sizeof(Textures));

    AppState* state = SDL_calloc(1, sizeof(AppState));
    if (!state) return SDL_APP_FAILURE;

    if (Assert(InitGraphicsData(graphicsDataContext), "Failed to initialise graphics data!\n")) return SDL_APP_FAILURE;
    if (Assert(LoadResources(graphicsDataContext, fonts, textures), "Failed to load resources!\n")) return SDL_APP_FAILURE;
    if (Assert(InitGameData(gameDataContext), "Failed to initialise game data!\n")) return SDL_APP_FAILURE;

    state->graphicsDataContext = graphicsDataContext;
    state->gameDataContext = gameDataContext;
    state->fonts = fonts;
    state->textures = textures;

    state->isRunning = true;
    *appstate = state;

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
    AppState* state = appstate;

    // TODO Fix delay after key repetition https://lazyfoo.net/tutorials/SDL/18_key_states/index.php
    // https://stackoverflow.com/questions/21311824/sdl2-key-repeat-delay

    switch (event->type)
    {
    case SDL_EVENT_QUIT:
        state->isRunning = false;
        break;

    case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
        SDL_Log("Window close requested");
        state->isRunning = false;
        break;

    case SDL_EVENT_WINDOW_RESIZED:
        ResizeGridSquares(state->graphicsDataContext, event->window.data1, event->window.data2);

        break;

    case SDL_EVENT_KEY_DOWN:

        switch (event->key.key)
        {
        case SDLK_D:
        case SDLK_RIGHT:
            ShiftTetromino(state->gameDataContext, 1);
            break;
        case SDLK_A:
        case SDLK_LEFT:
            ShiftTetromino(state->gameDataContext, -1);
            break;
        case SDLK_W:
        case SDLK_UP:
            WallKickDroppingTetromino(state->gameDataContext, 1);
            break;
        case SDLK_S:
        case SDLK_DOWN:
            SoftDropTetromino(state->gameDataContext);
            break;
        case SDLK_SPACE:
            HardDropTetromino(state->gameDataContext);
            break;
        default:
            break;
        }

        if (event->key.key == SDLK_ESCAPE)
        {
            SDL_Log("ESC pressed, exiting!");
            state->isRunning = false;
        }

        else
        {
            SDL_Log("Key: %s", SDL_GetKeyName(event->key.key));
        }
        break;

    default:
        break;
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate)
{
    const AppState* state = (AppState*)appstate;

    // Clear screen
    SDL_SetRenderDrawColor(state->graphicsDataContext->renderer, 17, 17, 17, 255);
    SDL_RenderClear(state->graphicsDataContext->renderer);

    // TODO Move all of these grapgics and rendering calls into one method in graphics.c
    if (Assert(DrawDroppingTetromino(state->graphicsDataContext, state->gameDataContext), "Failed to draw dropping tetromino!\n")) return SDL_APP_FAILURE;
    if (Assert(DrawDroppingTetrominoGhost(state->graphicsDataContext, state->gameDataContext), "Failed to draw dropping tetromino ghost!\n")) return SDL_APP_FAILURE;
    if (Assert(DrawArena(state->graphicsDataContext, state->gameDataContext), "Failed to draw arena!\n")) return SDL_APP_FAILURE;
    if (Assert(DrawSidebar(state->graphicsDataContext, state->fonts, state->textures, state->gameDataContext), "Failed to draw sidebar!\n")) return SDL_APP_FAILURE;
    DrawGameOverScreen(state->graphicsDataContext, state->fonts, state->textures, state->gameDataContext);
    if (Assert(SDL_RenderPresent(state->graphicsDataContext->renderer), "Failed to render previous draws!\n")) return SDL_APP_FAILURE;
    GameIteration(state->gameDataContext);
    

    return state->isRunning ? SDL_APP_CONTINUE : SDL_APP_SUCCESS; // return SDL_APP_SUCCESS to quit
}

void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
    SDL_Log("App Quit");

    if (appstate != NULL)
    {
        AppState* state = appstate;
        if (state->graphicsDataContext->renderer) SDL_DestroyRenderer(state->graphicsDataContext->renderer);
        if (state->graphicsDataContext->window) SDL_DestroyWindow(state->graphicsDataContext->window);
        SDL_free(state->gameDataContext->droppingTetromino);
        SDL_free(state->gameDataContext);
        SDL_free(state->graphicsDataContext);
        SDL_free(state->fonts);
        SDL_free(state->textures);
        SDL_free(state);
    }
}