#include "game.h"

#include <stdio.h>

#include "util.h"
#include "tetromino.h"
#include "graphics.h"

bool GameIteration(DroppingTetromino* droppingTetromino,
                   TetrominoIdentifier arena[ARENA_HEIGHT][ARENA_WIDTH])
{

	// Every n ticks, drop tetromino and run tetromino operations
	const int speed = 300;
	static Uint64 oldTick = 0;

	if (SDL_GetTicks() - oldTick >= speed)
	{
		ClearFilledRows(arena);

		// Check if dropping tetromino has connected with ground or another block, and mark for termination
		if (CheckDroppingTetrominoCollision(droppingTetromino, arena, droppingTetromino->x, droppingTetromino->y + 1) == true)
		{
			const int droppingTetrominoX = droppingTetromino->x;
			const int droppingTetrominoY = droppingTetromino->y;
			const bool (*droppingTetrominoRotatedCoordinates)[TETROMINO_MAX_SIZE] = droppingTetromino->shape.coordinates[droppingTetromino->rotation];

			// Update the arena with the location of the tetromino where it has collided
			for (int i = 0; i < TETROMINO_MAX_SIZE; i++)
			{
				for (int j = 0; j < TETROMINO_MAX_SIZE; j++)
				{
					printf("%d ", droppingTetrominoRotatedCoordinates[i][j]);

					if (droppingTetrominoRotatedCoordinates[i][j] == false) continue;
					
					arena[droppingTetrominoY + i][droppingTetrominoX + j] = droppingTetromino->shape.identifier;
				}
				printf("\n");
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
                                     const int x, 
                                     const int y)
{
	const bool (*droppingTetrominoRotatedCoordinates)[TETROMINO_MAX_SIZE] = droppingTetromino->shape.coordinates[droppingTetromino->rotation];

	printf("DT @ (%d,%d)\n", droppingTetromino->x, droppingTetromino->y);

	for (int i = 0; i < TETROMINO_MAX_SIZE; i++)
	{
		
		for (int j = 0; j < TETROMINO_MAX_SIZE; j++)
		{
			if (droppingTetrominoRotatedCoordinates[i][j] == false) continue;

			const int offsetX = x + j;
			const int offsetY = y + i;

			printf("DT offset coords = (%d,%d)\n", offsetX, offsetY);

			// Check if the tetromino has collided with the arena
			if (offsetX >= ARENA_WIDTH || offsetX < 0 || offsetY >= ARENA_HEIGHT || offsetY < 0) return true;

			// Check if the tetromino has collided with another tetromino on the board
			if (arena[offsetY][offsetX] != 0)
			{
				//SDL_Log("Block @(%d,%d) collision!", offsetX, offsetY);
				return true;
			}
		}
	}
	return false;
}

void ResetDroppingTetromino(DroppingTetromino* droppingTetromino)
{
	droppingTetromino->x = (ARENA_WIDTH - 1) / 2;
	droppingTetromino->y = 0;
	droppingTetromino->rotation = NORTH;
	droppingTetromino->shape = *GetTetrominoShapeByIdentifier(I);//*GetRandomTetrominoShape();
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


int ClearFilledRows(TetrominoIdentifier arena[ARENA_HEIGHT][ARENA_WIDTH])
{
	// Start scanning for filled rows from bottom of arena
	for (int row = ARENA_HEIGHT - 1; row >= 0; row--)
	{
		// The number of grid blocks that contain part of a tetromino in them
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
