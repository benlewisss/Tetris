#include <SDL3/SDL.h>

#include "../include/tetromino.h"

// Tetromino shape and color declaractions
const TetrominoShape PIECE_I =
{
    {0, 0, 255, 255},
    {0, 0, 0, -1, 0, 1, 0, 2},
};

const TetrominoShape PIECE_O =
{
    {255, 255, 0, 255},
    {0, 0, 1, 0, 0, 1, 1, 1},
};

const TetrominoShape PIECE_T =
{
    {128, 0, 128, 255},
    {0, 0, -1, 0, 1, 0, 0, 1},
};

const TetrominoShape PIECE_Z =
{
    {0, 255, 0, 255},
    {0, 0, -1, 0, 0, 1, 1, 1},
};

const TetrominoShape PIECE_S =
{
    {255, 0, 0, 255},
    {0, 0, 1, 0, 0, 1, -1, 1},
};

const TetrominoShape PIECE_L =
{
    {255, 165, 0, 255},
    {0, 0, 0, -1, 0, 1, 1, 1},
};

const TetrominoShape PIECE_J =
{
    {255, 192, 203, 255},
    {0, 0, 0, -1, 0, 1, -1, 1},
};

TetrominoShape get_random_tetromino_shape()
{
    TetrominoShape tetrominoes[TETROMINO_COUNT] = {PIECE_I, PIECE_O, PIECE_T, PIECE_Z, PIECE_S, PIECE_L, PIECE_J};
    return tetrominoes[SDL_rand(TETROMINO_COUNT)];
}