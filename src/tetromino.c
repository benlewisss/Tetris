#include <SDL3/SDL.h>

#include "tetromino.h"

// Tetromino shape and color declarations
TetrominoShape PIECE_I =
{
	I,
	NULL,
	{
		{1, 0, 1, 1, 1, 2, 1, 3},
		{0, 1, 1, 1, 2, 1, 3, 1},
		{1, 0, 1, 1, 1, 2, 1, 3},
		{0, 1, 1, 1, 2, 1, 3, 1},
	}
};

TetrominoShape PIECE_O =
{
	O,
	NULL,
	{
		{0, 0, 1, 0, 0, 1, 1, 1},
		{0, 0, 1, 0, 0, 1, 1, 1},
		{0, 0, 1, 0, 0, 1, 1, 1},
		{0, 0, 1, 0, 0, 1, 1, 1},
	}
};

TetrominoShape PIECE_T =
{
	T,
	NULL,
	{
		{1, 0, 0, 1, 1, 1, 2, 1},
		{1, 0, 1, 1, 2, 1, 1, 2},
		{0, 1, 1, 1, 2, 1, 1, 2},
		{1, 0, 0, 1, 1, 1, 1, 2},
	}
};

TetrominoShape PIECE_Z =
{
	Z,
	NULL,
	{
		{0, 1, 1, 1, 1, 2, 2, 2},
		{1, 0, 1, 1, 0, 1, 0, 2},
		{0, 0, 1, 0, 1, 1, 2, 1},
		{2, 0, 1, 1, 2, 1, 1, 2},
	}
};

TetrominoShape PIECE_S =
{
	S,
	NULL,
	{
		{1, 1, 2, 1, 0, 2, 1, 2},
		{0, 0, 0, 1, 1, 1, 1, 2},
		{1, 0, 2, 0, 0, 1, 1, 1},
		{1, 0, 1, 1, 2, 1, 2, 2},
	}
};

TetrominoShape PIECE_L =
{
	L,
	NULL,
	{
		{1, 0, 1, 1, 1, 2, 2, 2},
		{0, 1, 1, 1, 2, 1, 0, 2},
		{0, 0, 1, 0, 1, 1, 1, 2},
		{2, 0, 0, 1, 1, 1, 2, 1},
	}
};

TetrominoShape PIECE_J =
{
	J,
	NULL,
	{
		{1, 0, 1, 1, 0, 2, 1, 2},
		{0, 0, 0, 1, 1, 1, 2, 1},
		{1, 0, 2, 0, 1, 1, 1, 2},
		{0, 1, 1, 1, 2, 1, 2, 2},
	}
};

TetrominoShape* GetTetrominoShapeByIdentifier(const TetrominoIdentifier identifier)
{
	// Note: The order of this array must match the TetrominoIdentifier enum
	const TetrominoShape* tetrominoes[TETROMINO_COUNT] = {&PIECE_I, &PIECE_O, &PIECE_T, &PIECE_Z, &PIECE_S, &PIECE_L, &PIECE_J};
	return tetrominoes[identifier - 1]; // Identifiers are 1-indexed, array is 0-indexed
}

const TetrominoShape* GetRandomTetrominoShape(void)
{
	const TetrominoIdentifier identifier = (SDL_rand(TETROMINO_COUNT) + 1); // (Identifiers are 1-indexed)
	return GetTetrominoShapeByIdentifier(identifier);
}

void RotateDroppingTetromino(DroppingTetromino* tetromino, const int8_t rotationAmount)
{
	// TODO Is the following the most efficient way to do this? (No, maybe just use If statements to handle the negatives)
	const enum Orientation newDirection = (((tetromino->rotation + rotationAmount) % 4) + 4) % 4;
	// Cant do negative modulo operations in C
	tetromino->rotation = newDirection;
}
