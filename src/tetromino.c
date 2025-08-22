#include <SDL3/SDL.h>

#include "../include/tetromino.h"

// Tetromino shape and color declarations
const TetrominoShape PIECE_I =
{
    I,
    {0, 0, 255, 255},
    {
    	{1, 0, 1, 1, 1, 2, 1, 3},
		{0, 1, 1, 1, 2, 1, 3, 1},
		{1, 0, 1, 1, 1, 2, 1, 3},
        {0, 1, 1, 1, 2, 1, 3, 1},
    }
};

const TetrominoShape PIECE_O =
{
    O,
    {255, 255, 0, 255},
    {
        {0, 0, 1, 0, 0, 1, 1, 1},
        {0, 0, 1, 0, 0, 1, 1, 1},
        {0, 0, 1, 0, 0, 1, 1, 1},
        {0, 0, 1, 0, 0, 1, 1, 1},
    }
};

const TetrominoShape PIECE_T =
{
    T,
    {128, 0, 128, 255},
    {
        {1, 0, 0, 1, 1, 1, 2, 1},
        {1, 0, 1, 1, 2, 1, 1, 2},
        {0, 1, 1, 1, 2, 1, 1, 2},
        {1, 0, 0, 1, 1, 1, 1, 2},
    }
};

const TetrominoShape PIECE_Z =
{
    Z,
    {0, 255, 0, 255},
    {
        {0, 1, 1, 1, 1, 2, 2, 2},
        {1, 0, 1, 1, 0, 1, 0, 2},
        {0, 0, 1, 0, 1, 1, 2, 1},
        {2, 0, 1, 1, 2, 1, 1, 2},
    }
};

const TetrominoShape PIECE_S =
{
    S,
    {255, 0, 0, 255},
    {
        {1, 1, 2, 1, 0, 2, 1, 2},
        {0, 0, 0, 1, 1, 1, 1, 2},
        {1, 0, 2, 0, 0, 1, 1, 1},
        {1, 0, 1, 1, 2, 1, 2, 2},
    }
};

const TetrominoShape PIECE_L =
{
    L,
    {255, 165, 0, 255},
    {
        {1, 0, 1, 1, 1, 2, 2, 2},
        {0, 1, 1, 1, 2, 1, 0, 2},
        {0, 0, 1, 0, 1, 1, 1, 2},
        {2, 0, 0, 1, 1, 1, 2, 1},
    }
};

const TetrominoShape PIECE_J =
{
    J,
    {255, 192, 203, 255},
    {
        {1, 0, 1, 1, 0, 2, 1, 2},
        {0, 0, 0, 1, 1, 1, 2, 1},
        {1, 0, 2, 0, 1, 1, 1, 2},
        {0, 1, 1, 1, 2, 1, 2, 2},
    }
};

const TetrominoShape* get_tetromino_shape_by_identifier(const tetromino_identifier identifier)
{
    // Note: The order of this array must match the tetromino_identifier enum
    const TetrominoShape* tetrominoes[TETROMINO_COUNT] = { &PIECE_I, &PIECE_O, &PIECE_T, &PIECE_Z, &PIECE_S, &PIECE_L, &PIECE_J };
    return tetrominoes[identifier - 1]; // Identifiers are 1-indexed, array is 0-indexed
}

const TetrominoShape* get_random_tetromino_shape()
{
	const tetromino_identifier identifier = (SDL_rand(TETROMINO_COUNT) + 1); // (Identifiers are 1-indexed)
    return get_tetromino_shape_by_identifier(identifier);
}

bool rotate_tetromino(DroppingTetromino* tetromino, const int8_t rotation_amount)
{
    // TODO Is the following the most efficient way to do this? (No, maybe just use If statements to handle the negatives)
    const enum orientation new_direction = (((tetromino->rotation + rotation_amount) % 4) + 4) % 4; // Cant do negative modulo operations in C
    tetromino->rotation = new_direction;

    return true;
}