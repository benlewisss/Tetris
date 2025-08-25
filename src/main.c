#define SDL_MAIN_USE_CALLBACKS 1

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
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

static float g_blockSize;
static TetrominoIdentifier g_arena[ARENA_HEIGHT][ARENA_WIDTH] = {{0}};
static DroppingTetromino g_droppingTetromino;

//// DEBUG Purposes 
//static TetrominoIdentifier g_arena[ARENA_HEIGHT][ARENA_WIDTH] = {
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
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Couldn't initialize SDL!", SDL_GetError(), NULL);
		return SDL_APP_FAILURE;
	}

	AppState* state = SDL_calloc(1, sizeof(AppState));
	if (!state) return SDL_APP_FAILURE;

	// TODO Create window based off resolution
	SDL_CreateWindowAndRenderer("TETRIS", (ARENA_WIDTH * BLOCK_SIZE) + 200, ARENA_HEIGHT * BLOCK_SIZE, SDL_WINDOW_RESIZABLE, &state->window,&state->renderer);
	if (!state->window || !state->renderer)
	{
		SDL_Log("Window or Renderer creation failed: %s", SDL_GetError());
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Window or Renderer creation failed!", SDL_GetError(), NULL);
		SDL_DestroyWindow(state->window);
		return SDL_APP_FAILURE;
	}

	state->isRunning = true;
	*appstate = state;

	// TETRIS Init
	// Load resources, including assigning textures to tetrominoes
	if (!LoadResources(state->renderer))
	{
		SDL_Log("Loading resources failed: %s", SDL_GetError());
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Failed to load resources!", SDL_GetError(), NULL);
		return SDL_APP_FAILURE;
	}

	// TODO Block size should depend on size of window
	g_blockSize = 60;

	// Initialise the first dropping tetromino
	g_droppingTetromino.x = (ARENA_WIDTH - 1) / 2;
	g_droppingTetromino.y = 0;
	g_droppingTetromino.rotation = NORTH;
	g_droppingTetromino.shape = *GetRandomTetrominoShape();
	g_droppingTetromino.terminationTime = 0;

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
		const float widthBasedSize = event->window.data1 / (float) ARENA_WIDTH;
		const float heightBasedSize = event->window.data2 / (float) ARENA_HEIGHT;

		g_blockSize = (widthBasedSize < heightBasedSize) ? widthBasedSize : heightBasedSize;
		
		break;

	case SDL_EVENT_KEY_DOWN:

		switch (event->key.key)
		{
		case SDLK_D:
		case SDLK_RIGHT:
			if (!CheckDroppingTetrominoCollision(g_arena, &g_droppingTetromino, 1, 0, 0)) g_droppingTetromino.x++;
			break;
		case SDLK_A:
		case SDLK_LEFT:
			if (!CheckDroppingTetrominoCollision(g_arena, &g_droppingTetromino, -1, 0, 0)) g_droppingTetromino.x--;
			break;
		case SDLK_W:
		case SDLK_UP:
			WallKickRotateDroppingTetromino(g_arena, &g_droppingTetromino, 1);
			break;
		case SDLK_S:
		case SDLK_DOWN:
			/*WallKickRotateDroppingTetromino(g_arena, &g_droppingTetromino, -1);*/
			SoftDropTetromino(g_arena, &g_droppingTetromino);
			break;
		case SDLK_SPACE:
			HardDropTetromino(g_arena, &g_droppingTetromino);
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

	default:
		break;
	}

	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate)
{
	const AppState* state = (AppState*)appstate;

	// Clear screen
	SDL_SetRenderDrawColor(state->renderer, 17, 17, 17, 255);
	SDL_RenderClear(state->renderer);

	// TODO If I am passing blockSize and arena every time to different draw calls, might it not be better to have this as a global within graphics which we just modify with setters in main()?
	DrawDroppingTetromino(state->renderer, g_blockSize, &g_droppingTetromino);
	DrawDroppingTetrominoGhost(state->renderer, g_blockSize, g_arena, &g_droppingTetromino);
	DrawArena(state->renderer, g_blockSize, g_arena);

	GameIteration(g_arena, &g_droppingTetromino);

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
