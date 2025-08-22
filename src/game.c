#include "game.h"
#include "util.h"
#include "tetromino.h"
#include "graphics.h"

bool GameIteration(SDL_Renderer* renderer, DroppingTetromino* droppingTetromino,
                   TetrominoIdentifier arena[ARENA_HEIGHT][ARENA_WIDTH])
{
	DrawArena(renderer, arena);

	DrawDroppingTetromino(renderer, droppingTetromino);

	// Every n ticks, drop tetromino and run tetromino operations
	const uint16_t speed = 200;
	static Uint64 oldTick = 0;

	if (SDL_GetTicks() - oldTick >= speed)
	{
		ClearFilledRows(arena);

		// Check if dropping tetromino has connected with ground or another block, and mark for termination
		if (CheckDroppingTetrominoCollision(droppingTetromino, arena, droppingTetromino->x, droppingTetromino->y + 1) == true)
		{
			// Update the arena with the location of the tetromino where it has collided
			for (int i = 0; i <= (TETROMINO_SIZE - 1) * 2; i += 2)
			{
				const int offsetX = droppingTetromino->x + droppingTetromino->shape.offsets[droppingTetromino->rotation][i];
				const int offsetY = droppingTetromino->y + droppingTetromino->shape.offsets[droppingTetromino->rotation][i + 1];

				arena[offsetY][offsetX] = droppingTetromino->shape.identifier;
			}

			// If the dropping tetromino in the previous iteration is marked for termination, that means we must replace it
			// with a new dropping tetromino, essentially "spawning" a new one
			ResetDroppingTetromino(droppingTetromino);
			return true;
		}

		droppingTetromino->y++;

		oldTick = SDL_GetTicks();
	}

	// Check if a row is filled and destroy it, increment score

	return true;
}

bool CheckDroppingTetrominoCollision(const DroppingTetromino* droppingTetromino,
                                     const TetrominoIdentifier arena[ARENA_HEIGHT][ARENA_WIDTH],
                                     const int8_t x,
                                     const int8_t y)
{
	SDL_Log("DT @(%d,%d): Block(%d,%d,%d,%d,%d,%d,%d,%d)",
	        droppingTetromino->x,
	        droppingTetromino->y,
	        droppingTetromino->x + droppingTetromino->shape.offsets[droppingTetromino->rotation][0],
	        droppingTetromino->y + droppingTetromino->shape.offsets[droppingTetromino->rotation][1],
	        droppingTetromino->x + droppingTetromino->shape.offsets[droppingTetromino->rotation][2],
	        droppingTetromino->y + droppingTetromino->shape.offsets[droppingTetromino->rotation][3],
	        droppingTetromino->x + droppingTetromino->shape.offsets[droppingTetromino->rotation][4],
	        droppingTetromino->y + droppingTetromino->shape.offsets[droppingTetromino->rotation][5],
	        droppingTetromino->x + droppingTetromino->shape.offsets[droppingTetromino->rotation][6],
	        droppingTetromino->y + droppingTetromino->shape.offsets[droppingTetromino->rotation][7]);

	// Iterate over every other offset in the tetromino shape (to differentiate x, y coords)
	for (int i = 0; i <= (TETROMINO_SIZE - 1) * 2; i += 2)
	{
		const int8_t offsetX = x + droppingTetromino->shape.offsets[droppingTetromino->rotation][i];
		const int8_t offsetY = y + droppingTetromino->shape.offsets[droppingTetromino->rotation][i + 1];

		// Check if the tetromino has collided with the arena
		if (offsetX >= ARENA_WIDTH || offsetX < 0 || offsetY >= ARENA_HEIGHT || offsetY < 0) return true;

		// Check if the tetromino has collided with another tetromino on the board
		if (arena[offsetY][offsetX] != 0)
		{
			SDL_Log("Block @(%d,%d) collision!", offsetX, offsetY);
			return true;
		}
	}

	return false;
}

void ResetDroppingTetromino(DroppingTetromino* droppingTetromino)
{
	droppingTetromino->x = (ARENA_WIDTH - 1) / 2;
	droppingTetromino->y = 0;
	droppingTetromino->rotation = NORTH;
	droppingTetromino->shape = *GetRandomTetrominoShape();
	droppingTetromino->terminate = false;
}

/* Drops everything above this row by one
 *
*/
static void DropRows(TetrominoIdentifier arena[ARENA_HEIGHT][ARENA_WIDTH], const int dropToRow)
{
	// Start scanning for filled rows from bottom of arena
	for (int row = dropToRow; row > 0; row--)
	{
		for (int col = 0; col < ARENA_WIDTH; col++)
		{
			arena[row][col] = arena[row - 1][col];
		}
	}
}


uint16_t ClearFilledRows(TetrominoIdentifier arena[ARENA_HEIGHT][ARENA_WIDTH])
{
	// Start scanning for filled rows from bottom of arena
	for (int row = ARENA_HEIGHT - 1; row >= 0; row--)
	{
		// The number of grid squares that contain part of a tetromino in them
		int gridFillCount = 0;
		for (int col = 0; col < ARENA_WIDTH; col++)
		{
			if (arena[row][col] != 0) gridFillCount++;
		}

		// If we encounter an empty row while scanning upwards, then there can't be a filled row above that point
		if (gridFillCount == 0)
		{
			break;
		}

		if (gridFillCount >= ARENA_WIDTH)
		{
			// Clear row
			memset(arena[row], 0, ARENA_WIDTH);
			DropRows(arena, row);
		}
		// TODO Maybe change the colour of the row 1 frame before we delete it, as some sort of animation?
	}

	return 0;
}
