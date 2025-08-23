#include <SDL3/SDL.h>

#include "tetromino.h"

// Tetromino shape and color declarations
TetrominoShape PIECE_I =
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

TetrominoShape PIECE_O =
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

TetrominoShape PIECE_T =
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

TetrominoShape PIECE_Z =
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

TetrominoShape PIECE_S =
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

TetrominoShape PIECE_L =
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

TetrominoShape PIECE_J =
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

TetrominoShape* GetTetrominoShapeByIdentifier(const TetrominoIdentifier identifier)
{
	// Note: The order of this array must match the TetrominoIdentifier enum
	TetrominoShape* tetrominoes[TETROMINO_COUNT] = {&PIECE_I, &PIECE_O, &PIECE_T, &PIECE_Z, &PIECE_S, &PIECE_L, &PIECE_J};
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
