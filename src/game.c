#include "game.h"

#include "util.h"
#include "tetromino.h"

bool GameIteration(TetrominoIdentifier arena[ARENA_HEIGHT][ARENA_WIDTH],
                   DroppingTetromino* droppingTetromino)
{
	static int score = 0;

	// Every n ticks, drop tetromino and run tetromino operations
	const int speed = 750;
	static Uint64 oldTick = 0;

	if (SDL_GetTicks() - oldTick >= speed)
	{
		score += ClearFilledRows(arena);

		// Check if dropping tetromino has connected with ground or another block, and mark for termination
		if (CheckDroppingTetrominoCollision(arena, droppingTetromino, 0, 1, 0) == true)
		{
			const int droppingTetrominoX = droppingTetromino->x;
			const int droppingTetrominoY = droppingTetromino->y;
			const bool (*droppingTetrominoRotatedCoordinates)[TETROMINO_MAX_SIZE] = droppingTetromino->shape.coordinates[droppingTetromino->rotation];

			// Update the arena with the location of the tetromino where it has collided
			for (int i = 0; i < TETROMINO_MAX_SIZE; i++)
			{
				for (int j = 0; j < TETROMINO_MAX_SIZE; j++)
				{
					if (droppingTetrominoRotatedCoordinates[i][j] == false) continue;
					arena[droppingTetrominoY + i][droppingTetrominoX + j] = droppingTetromino->shape.identifier;
				}
			}

			// If the dropping tetromino in the previous iteration is marked for termination, that means we must replace it
			// with a new dropping tetromino, essentially "spawning" a new one
			ResetDroppingTetromino(droppingTetromino);
			return true;
		}

		droppingTetromino->y++;

		oldTick = SDL_GetTicks();
	}

	return true;
}

bool CheckDroppingTetrominoCollision(const TetrominoIdentifier arena[ARENA_HEIGHT][ARENA_WIDTH], const DroppingTetromino* droppingTetromino, int translationX, int translationY, const int rotationAmount)
{
	const bool (*droppingTetrominoRotatedCoordinates)[TETROMINO_MAX_SIZE] = droppingTetromino->shape.coordinates[(((droppingTetromino->rotation + rotationAmount) % 4) + 4) % 4];

	translationX += droppingTetromino->x;
	translationY += droppingTetromino->y;

	for (int i = 0; i < TETROMINO_MAX_SIZE; i++)
	{
		for (int j = 0; j < TETROMINO_MAX_SIZE; j++)
		{
			if (droppingTetrominoRotatedCoordinates[i][j] == false) continue;

			const int offsetX = translationX + j;
			const int offsetY = translationY + i;

			// Check if the tetromino has collided with the arena
			if (offsetX >= ARENA_WIDTH || offsetX < 0 || offsetY >= ARENA_HEIGHT || offsetY < 0) return true;

			// Check if the tetromino has collided with another tetromino on the board
			if (arena[offsetY][offsetX] != 0)
			{
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
	droppingTetromino->shape = *GetTetrominoShapeByIdentifier(J);//*GetRandomTetrominoShape();
	droppingTetromino->terminate = false;
}

/* Drops everything above this row by drop amount
 *
*/
static void DropRows(TetrominoIdentifier arena[ARENA_HEIGHT][ARENA_WIDTH], const int dropToRow, const int dropAmount)
{
	// Start scanning for filled rows from bottom of arena
	for (int row = dropToRow; row > 0; row--)
	{
		int squareCount = 0;
		for (int col = 0; col < ARENA_WIDTH; col++)
		{
			if (arena[row][col] != 0) squareCount++;
			arena[row][col] = arena[row - dropAmount][col];
		}

		// If we reach an empty row, then nothing above it to drop down
		if (squareCount == 0) break;
	}
}

int ClearFilledRows(TetrominoIdentifier arena[ARENA_HEIGHT][ARENA_WIDTH])
{
	// Number of sequential rows that have been filled
	int numFilledRows = 0;

	// Sliding window technique
	int bottomPointer = ARENA_HEIGHT - 1;
	int topPointer = ARENA_HEIGHT - 2;

	while (topPointer < bottomPointer && topPointer >= 0)
	{
		int bottomPointerSquareCount = 0;
		int topPointerSquareCount = 0;

		for (int col = 0; col < ARENA_WIDTH; col++)
		{
			if (arena[bottomPointer][col] != 0) bottomPointerSquareCount++;
			if (arena[topPointer][col] != 0) topPointerSquareCount++;
		}

		if (topPointerSquareCount < ARENA_WIDTH && bottomPointerSquareCount < ARENA_WIDTH)
		{
			topPointer--;
			bottomPointer--;
			continue;
		}

		if (topPointerSquareCount >= ARENA_WIDTH)
		{ 
			topPointer--;
		}
		else
		{
			numFilledRows = bottomPointer - topPointer;
			break;
		}

		if (bottomPointerSquareCount < ARENA_WIDTH)
		{
			bottomPointer--;
		}
	}

	// Clear the cleared rows and drop the rows above
	for (int row = bottomPointer; row <= topPointer; row--)
	{
		memset(arena[row], 0, ARENA_WIDTH);
	}
	DropRows(arena, bottomPointer, bottomPointer - topPointer);

	return numFilledRows * 1000;
}

void WallKickRotateDroppingTetromino(TetrominoIdentifier arena[ARENA_HEIGHT][ARENA_WIDTH], DroppingTetromino* droppingTetromino, const int rotationAmount)
{
	//// I think the best way is to store the Wall Kick Data (the 2d array can be found at https://tetris.wiki/Super_Rotation_System) as a 2D array of coordinate pairs (so 3D array), which is accessed here.
	//if (rotationAmount == -1)
	//{
	//	if (CheckDroppingTetrominoRotationCollision(arena, droppingTetromino,))
	//}

	//RotateDroppingTetromino(DroppingTetromino)
}