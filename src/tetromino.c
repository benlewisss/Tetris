#include <SDL3/SDL.h>

#include "tetromino.h"

TetrominoShape* GetTetrominoShapeByIdentifier(const TetrominoIdentifier identifier)
{
	// Tetromino shape and color declarations
	static TetrominoShape pieceI =
	{
		I,
		NULL,
		{
			{{0, 0, 0, 0},
				{1, 1, 1, 1},
				{0, 0, 0, 0},
				{0, 0, 0, 0}},
			{{0, 0, 1, 0},
				{0, 0, 1, 0},
				{0, 0, 1, 0},
				{0, 0, 1, 0}},
			{{0, 0, 0, 0},
				{0, 0, 0, 0},
				{1, 1, 1, 1},
				{0, 0, 0, 0}},
			{{0, 1, 0, 0},
				{0, 1, 0, 0},
				{0, 1, 0, 0},
				{0, 1, 0, 0}},
		}

	};

	static TetrominoShape pieceO =
	{
		O,
		NULL,
		{
			{{0, 1, 1, 0},
				{0, 1, 1, 0},
				{0, 0, 0, 0},
				{0, 0, 0, 0}},
			{{0, 1, 1, 0},
				{0, 1, 1, 0},
				{0, 0, 0, 0},
				{0, 0, 0, 0}},
			{{0, 1, 1, 0},
				{0, 1, 1, 0},
				{0, 0, 0, 0},
				{0, 0, 0, 0}},
			{{0, 1, 1, 0},
				{0, 1, 1, 0},
				{0, 0, 0, 0},
				{0, 0, 0, 0}},
		}
	};

	static TetrominoShape pieceT =
	{
		T,
		NULL,
		{
			{{0, 1, 0, 0},
				{1, 1, 1, 0},
				{0, 0, 0, 0},
				{0, 0, 0, 0}},
			{{0, 1, 0, 0},
				{0, 1, 1, 0},
				{0, 1, 0, 0},
				{0, 0, 0, 0}},
			{{0, 0, 0, 0},
				{1, 1, 1, 0},
				{0, 1, 0, 0},
				{0, 0, 0, 0}},
			{{0, 1, 0, 0},
				{1, 1, 0, 0},
				{0, 1, 0, 0},
				{0, 0, 0, 0}},
		}
	};

	static TetrominoShape pieceZ =
	{
		Z,
		NULL,
		{
			{{1, 1, 0, 0},
				{0, 1, 1, 0},
				{0, 0, 0, 0},
				{0, 0, 0, 0}},
			{{0, 0, 1, 0},
				{0, 1, 1, 0},
				{0, 1, 0, 0},
				{0, 0, 0, 0}},
			{{0, 0, 0, 0},
				{1, 1, 0, 0},
				{0, 1, 1, 0},
				{0, 0, 0, 0}},
			{{0, 1, 0, 0},
				{1, 1, 0, 0},
				{1, 0, 0, 0},
				{0, 0, 0, 0}},
		}
	};

	static TetrominoShape pieceS =
	{
		S,
		NULL,
		{
			{{0, 1, 1, 0},
				{1, 1, 0, 0},
				{0, 0, 0, 0},
				{0, 0, 0, 0}},
			{{0, 1, 0, 0},
				{0, 1, 1, 0},
				{0, 0, 1, 0},
				{0, 0, 0, 0}},
			{{0, 0, 0, 0},
				{0, 1, 1, 0},
				{1, 1, 0, 0},
				{0, 0, 0, 0}},
			{{1, 0, 0, 0},
				{1, 1, 0, 0},
				{0, 1, 0, 0},
				{0, 0, 0, 0}},
		}
	};

	static TetrominoShape pieceL =
	{
		L,
		NULL,
		{
			{{0, 0, 1, 0},
				{1, 1, 1, 0},
				{0, 0, 0, 0},
				{0, 0, 0, 0}},
			{{0, 1, 0, 0},
				{0, 1, 0, 0},
				{0, 1, 1, 0},
				{0, 0, 0, 0}},
			{{0, 0, 0, 0},
				{1, 1, 1, 0},
				{1, 0, 0, 0},
				{0, 0, 0, 0}},
			{{1, 1, 0, 0},
				{0, 1, 0, 0},
				{0, 1, 0, 0},
				{0, 0, 0, 0}},
		}
	};

	static TetrominoShape pieceJ =
	{
		J,
		NULL,
		{
			{{1, 0, 0, 0},
				{1, 1, 1, 0},
				{0, 0, 0, 0},
				{0, 0, 0, 0}},
			{{0, 1, 1, 0},
				{0, 1, 0, 0},
				{0, 1, 0, 0},
				{0, 0, 0, 0}},
			{{0, 0, 0, 0},
				{1, 1, 1, 0},
				{0, 0, 1, 0},
				{0, 0, 0, 0}},
			{{0, 1, 0, 0},
				{0, 1, 0, 0},
				{1, 1, 0, 0},
				{0, 0, 0, 0}},
		}
	};

	// Note: The order of this array must match the TetrominoIdentifier enum
	TetrominoShape* tetrominoes[TETROMINO_COUNT] = {&pieceI, &pieceO, &pieceT, &pieceZ, &pieceS, &pieceL, &pieceJ};
	return tetrominoes[identifier - 1]; // Identifiers are 1-indexed, array is 0-indexed
}

const TetrominoShape* GetRandomTetrominoShape(void)
{
	const TetrominoIdentifier identifier = (SDL_rand(TETROMINO_COUNT) + 1); // (Identifiers are 1-indexed)
	return GetTetrominoShapeByIdentifier(identifier);
}

void RotateDroppingTetromino(DroppingTetromino* droppingTetromino, const int rotationAmount)
{
	// TODO Is the following the most efficient way to do this? (No, maybe just use If statements to handle the negatives)
	const enum Orientation newDirection = (((droppingTetromino->rotation + rotationAmount) % 4) + 4) % 4;
	// Cant do negative modulo operations in C
	droppingTetromino->rotation = newDirection;
}
