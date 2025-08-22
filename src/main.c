#define SDL_MAIN_USE_CALLBACKS 1

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "../include/util.h"
#include "../include/tetromino.h"
#include "../include/graphics.h"
#include "../include/game.h"

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

static TetrominoIdentifier g_arena[ARENA_HEIGHT][ARENA_WIDTH] = {{0}};
static DroppingTetromino g_droppingTetromino;

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
	SDL_Log("Tetris Initialisation");

	// Setup application metadata
	if (!SDL_SetAppMetadata("TETRIS", "1.0", "Tetris"))
	{
		return SDL_APP_FAILURE;
	}

	for (size_t i = 0; i < SDL_arraysize(EXTENDED_METADATA); i++)
	{
		if (!SDL_SetAppMetadataProperty(EXTENDED_METADATA[i].key, EXTENDED_METADATA[i].value))
		{
			return SDL_APP_FAILURE;
		}
	}

	if (!SDL_Init(SDL_INIT_VIDEO))
	{
		SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
		// SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Couldn't initialize SDL!", SDL_GetError(), NULL);
		return SDL_APP_FAILURE;
	}

	AppState* state = SDL_calloc(1, sizeof(AppState));
	if (!state) return SDL_APP_FAILURE;

	SDL_CreateWindowAndRenderer("TETRIS", ARENA_WIDTH * BLOCK_SIZE, ARENA_HEIGHT * BLOCK_SIZE, false, &state->window,
	                            &state->renderer);
	if (!state->window || !state->renderer)
	{
		SDL_Log("Window or Renderer creation failed: %s", SDL_GetError());
		SDL_DestroyWindow(state->window);
		return SDL_APP_FAILURE;
	}

	state->isRunning = true;
	*appstate = state;

	// TETRIS Init
	// Initialise the first dropping tetromino
	ResetDroppingTetromino(&g_droppingTetromino);

	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
	AppState* state = appstate;

	switch (event->type)
	{
	case SDL_EVENT_QUIT:
		state->isRunning = false;
		break;

	case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
		SDL_Log("Window close requested");
		state->isRunning = false;
		break;

	case SDL_EVENT_KEY_DOWN:

		switch (event->key.key)
		{
		case SDLK_D:
		case SDLK_RIGHT:
			if (!CheckDroppingTetrominoCollision(&g_droppingTetromino, g_arena, g_droppingTetromino.x + 1,
			                                     g_droppingTetromino.y)) g_droppingTetromino.x++;
			break;
		case SDLK_A:
		case SDLK_LEFT:
			if (!CheckDroppingTetrominoCollision(&g_droppingTetromino, g_arena, g_droppingTetromino.x - 1,
			                                     g_droppingTetromino.y)) g_droppingTetromino.x--;
			break;
		case SDLK_W:
		case SDLK_UP: // TODO There's a BIG bug where you can rotate at the edges and clip into the sides
			if (!CheckDroppingTetrominoCollision(&g_droppingTetromino, g_arena, g_droppingTetromino.x + 1,
			                                     g_droppingTetromino.y)) RotateDroppingTetromino(
				&g_droppingTetromino, 1);
			break;
		case SDLK_S:
		case SDLK_DOWN:
			if (!CheckDroppingTetrominoCollision(&g_droppingTetromino, g_arena, g_droppingTetromino.x - 1,
			                                     g_droppingTetromino.y)) RotateDroppingTetromino(
				&g_droppingTetromino, -1);
			break;
		default:
			break;
		}

		if (event->key.key == SDLK_ESCAPE)
		{
			SDL_Log("ESC pressed, exiting");
			state->isRunning = false;
		}

		else
		{
			SDL_Log("Key: %s", SDL_GetKeyName(event->key.key));
		}
		break;

	case SDL_EVENT_WINDOW_RESIZED:
		SDL_Log("Window Resized to %d x %d", event->window.data1, event->window.data2);
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
	SDL_SetRenderDrawColor(state->renderer, 0, 0, 0, 255);
	SDL_RenderClear(state->renderer);

	GameIteration(state->renderer, &g_droppingTetromino, g_arena);

	// int width, height;
	// SDL_GetWindowSize(state->window, &width, &height);

	SDL_RenderPresent(state->renderer);

	return state->isRunning ? SDL_APP_CONTINUE : SDL_APP_SUCCESS; // return SDL_APP_SUCCESS to quit
}

void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
	SDL_Log("App Quit");

	if (appstate != NULL)
	{
		AppState* state = appstate;
		if (state->renderer) SDL_DestroyRenderer(state->renderer);
		if (state->window) SDL_DestroyWindow(state->window);
		SDL_free(state);
	}
}
