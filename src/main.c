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
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool isRunning;
} AppState;



//// DEBUG Purposes 
//static TetrominoIdentifier mainArena[ARENA_HEIGHT][ARENA_WIDTH] = {
//{0,0,0,0,0,0,0,0,0,0},
//{0,0,0,0,0,0,0,0,0,0},
//{0,0,0,0,0,0,0,0,0,0},
//{0,0,0,0,0,0,0,0,0,0},
//{0,0,0,0,0,0,0,0,0,0},
//{0,0,0,0,0,0,0,0,0,0},
//{0,0,0,0,0,0,0,0,0,0},
//{0,0,0,0,0,0,0,0,0,0},
//{0,0,0,0,0,0,0,0,0,0},
//{0,0,0,0,0,0,0,0,0,0},
//{0,0,0,0,0,0,0,0,0,0},
//{0,0,0,0,0,0,0,0,0,0},
//{0,0,0,0,0,0,0,0,0,0},
//{0,0,0,0,1,1,0,0,0,0},
//{0,0,0,0,0,1,1,1,0,0},
//{0,0,0,0,0,0,1,1,1,1},
//{0,1,1,1,0,0,0,1,1,1},
//{1,1,0,0,0,0,1,1,1,1},
//{1,1,1,1,0,0,1,1,1,1},
//{1,1,1,1,1,0,1,1,1,1}, };

// NOTE: Initialising the first value to zero actually initialises the entire struct to zero, including the arena
// attribute, hence avoiding undefined behaviour.
static GameDataContext gameDataContext = { 0 };
static GraphicsDataContext graphicsDataContext = { 0 }; // TODO Include a pointer to these in Appstate, so they don't need to be global. You may need to SDL_Calloc them.

static Fonts fonts;
static Textures textures;

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
    // Setup application metadata
    if (Assert(SDL_SetAppMetadata("TETRIS", "1.0", "Tetris"), "Failed to initialise app metadata!\n")) return SDL_APP_FAILURE;

    for (size_t i = 0; i < SDL_arraysize(EXTENDED_METADATA); i++)
    {
        if (Assert(SDL_SetAppMetadataProperty(EXTENDED_METADATA[i].key, EXTENDED_METADATA[i].value), "Failed to set metadata properties!\n")) return SDL_APP_FAILURE;
    }

    if (Assert(SDL_Init(SDL_INIT_VIDEO), "Failed to initialise SDL!\n")) return SDL_APP_FAILURE;
    if (Assert(TTF_Init(), "Failed to initialise TTF!\n")) return SDL_APP_FAILURE;

    AppState* state = SDL_calloc(1, sizeof(AppState));
    if (!state) return SDL_APP_FAILURE;

    if (Assert(InitGraphicsData(&graphicsDataContext), "Failed to initialise graphics data!\n")) return SDL_APP_FAILURE;

    state->isRunning = true;
    *appstate = state;

    if (Assert(InitGameData(&gameDataContext), "Failed to initialise game data!\n")) return SDL_APP_FAILURE;

    if (Assert(LoadResources(&graphicsDataContext, &fonts, &textures), "Failed to load resources!\n")) return SDL_APP_FAILURE;
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
        ResizeGridSquares(&graphicsDataContext, event->window.data1, event->window.data2);

        break;

    case SDL_EVENT_KEY_DOWN:

        switch (event->key.key)
        {
        case SDLK_D:
        case SDLK_RIGHT:
            ShiftTetromino(&gameDataContext, 1);
            break;
        case SDLK_A:
        case SDLK_LEFT:
            ShiftTetromino(&gameDataContext, -1);
            break;
        case SDLK_W:
        case SDLK_UP:
            WallKickDroppingTetromino(&gameDataContext, 1);
            break;
        case SDLK_S:
        case SDLK_DOWN:
            SoftDropTetromino(&gameDataContext);
            break;
        case SDLK_SPACE:
            HardDropTetromino(&gameDataContext);
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
    SDL_SetRenderDrawColor(graphicsDataContext.renderer, 17, 17, 17, 255);
    SDL_RenderClear(graphicsDataContext.renderer);

    // TODO If I am passing blockSize and arena every time to different draw calls, might it not be better to have this as a global within graphics which we just modify with setters in main()?

    if (Assert(DrawDroppingTetromino(&graphicsDataContext, &gameDataContext), "Failed to draw dropping tetromino!\n")) return SDL_APP_FAILURE;
    if (Assert(DrawDroppingTetrominoGhost(&graphicsDataContext, &gameDataContext), "Failed to draw dropping tetromino ghost!\n")) return SDL_APP_FAILURE;
    if (Assert(DrawArena(&graphicsDataContext, &gameDataContext), "Failed to draw arena!\n")) return SDL_APP_FAILURE;
    if (Assert(DrawSidebar(&graphicsDataContext, &fonts, &textures, &gameDataContext), "Failed to draw sidebar!\n")) return SDL_APP_FAILURE;
    if (Assert(SDL_RenderPresent(graphicsDataContext.renderer), "Failed to render previous draws!\n")) return SDL_APP_FAILURE;
    GameIteration(&gameDataContext);
    

    return state->isRunning ? SDL_APP_CONTINUE : SDL_APP_SUCCESS; // return SDL_APP_SUCCESS to quit
}

void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
    SDL_Log("App Quit");

    if (appstate != NULL)
    {
        AppState* state = appstate;
        if (graphicsDataContext.renderer) SDL_DestroyRenderer(graphicsDataContext.renderer);
        if (graphicsDataContext.window) SDL_DestroyWindow(graphicsDataContext.window);
        SDL_free(state);
    }
}