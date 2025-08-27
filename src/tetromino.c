#include <SDL3/SDL.h>
#include <stdlib.h>

#include "tetromino.h"

TetrominoShape* GetTetrominoShapeByIdentifier(const TetrominoIdentifier identifier)
{
    // Tetromino shape and color declarations
    static TetrominoShape pieceI =
    {
        I,
        NULL,
        {
            {
                {0, 0, 0, 0},
                {1, 1, 1, 1},
                {0, 0, 0, 0},
                {0, 0, 0, 0}
            },
            {
                {0, 0, 1, 0},
                {0, 0, 1, 0},
                {0, 0, 1, 0},
                {0, 0, 1, 0}
            },
            {
                {0, 0, 0, 0},
                {0, 0, 0, 0},
                {1, 1, 1, 1},
                {0, 0, 0, 0}
            },
            {
                {0, 1, 0, 0},
                {0, 1, 0, 0},
                {0, 1, 0, 0},
                {0, 1, 0, 0}
            },
        }

    };

    static TetrominoShape pieceO =
    {
        O,
        NULL,
        {
            {
                {0, 1, 1, 0},
                {0, 1, 1, 0},
                {0, 0, 0, 0},
                {0, 0, 0, 0}
            },
            {
                {0, 1, 1, 0},
                {0, 1, 1, 0},
                {0, 0, 0, 0},
                {0, 0, 0, 0}
            },
            {
                {0, 1, 1, 0},
                {0, 1, 1, 0},
                {0, 0, 0, 0},
                {0, 0, 0, 0}
            },
            {
                {0, 1, 1, 0},
                {0, 1, 1, 0},
                {0, 0, 0, 0},
                {0, 0, 0, 0}
            },
        }
    };

    static TetrominoShape pieceT =
    {
        T,
        NULL,
        {
            {
                {0, 1, 0, 0},
                {1, 1, 1, 0},
                {0, 0, 0, 0},
                {0, 0, 0, 0}
            },
            {
                {0, 1, 0, 0},
                {0, 1, 1, 0},
                {0, 1, 0, 0},
                {0, 0, 0, 0}
            },
            {
                {0, 0, 0, 0},
                {1, 1, 1, 0},
                {0, 1, 0, 0},
                {0, 0, 0, 0}
            },
            {
                {0, 1, 0, 0},
                {1, 1, 0, 0},
                {0, 1, 0, 0},
                {0, 0, 0, 0}
            },
        }
    };

    static TetrominoShape pieceZ =
    {
        Z,
        NULL,
        {
            {
                {1, 1, 0, 0},
                {0, 1, 1, 0},
                {0, 0, 0, 0},
                {0, 0, 0, 0}
            },
            {
                {0, 0, 1, 0},
                {0, 1, 1, 0},
                {0, 1, 0, 0},
                {0, 0, 0, 0}
            },
            {
                {0, 0, 0, 0},
                {1, 1, 0, 0},
                {0, 1, 1, 0},
                {0, 0, 0, 0}
            },
            {
                {0, 1, 0, 0},
                {1, 1, 0, 0},
                {1, 0, 0, 0},
                {0, 0, 0, 0}
            },
        }
    };

    static TetrominoShape pieceS =
    {
        S,
        NULL,
        {
            {
                {0, 1, 1, 0},
                {1, 1, 0, 0},
                {0, 0, 0, 0},
                {0, 0, 0, 0}
            },
            {
                {0, 1, 0, 0},
                {0, 1, 1, 0},
                {0, 0, 1, 0},
                {0, 0, 0, 0}
            },
            {
                {0, 0, 0, 0},
                {0, 1, 1, 0},
                {1, 1, 0, 0},
                {0, 0, 0, 0}
            },
            {
                {1, 0, 0, 0},
                {1, 1, 0, 0},
                {0, 1, 0, 0},
                {0, 0, 0, 0}
            },
        }
    };

    static TetrominoShape pieceL =
    {
        L,
        NULL,
        {
            {
                {0, 0, 1, 0},
                {1, 1, 1, 0},
                {0, 0, 0, 0},
                {0, 0, 0, 0}
            },
            {
                {0, 1, 0, 0},
                {0, 1, 0, 0},
                {0, 1, 1, 0},
                {0, 0, 0, 0}
            },
            {
                {0, 0, 0, 0},
                {1, 1, 1, 0},
                {1, 0, 0, 0},
                {0, 0, 0, 0}
            },
            {
                {1, 1, 0, 0},
                {0, 1, 0, 0},
                {0, 1, 0, 0},
                {0, 0, 0, 0}
            },
        }
    };

    static TetrominoShape pieceJ =
    {
        J,
        NULL,
        {
            {
                {1, 0, 0, 0},
                {1, 1, 1, 0},
                {0, 0, 0, 0},
                {0, 0, 0, 0}
            },
            {
                {0, 1, 1, 0},
                {0, 1, 0, 0},
                {0, 1, 0, 0},
                {0, 0, 0, 0}
            },
            {
                {0, 0, 0, 0},
                {1, 1, 1, 0},
                {0, 0, 1, 0},
                {0, 0, 0, 0}
            },
            {
                {0, 1, 0, 0},
                {0, 1, 0, 0},
                {1, 1, 0, 0},
                {0, 0, 0, 0}
            },
        }
    };

    // Note: The order of this array must match the TetrominoIdentifier enum
    TetrominoShape* tetrominoes[TETROMINO_COUNT] = {&pieceI, &pieceO, &pieceT, &pieceZ, &pieceS, &pieceL, &pieceJ};

    return tetrominoes[identifier - 1]; // Identifiers are 1-indexed, array is 0-indexed
}

const TetrominoShape* GetRandomTetrominoShape(void)
{
    static int dropCount = 0;
    static TetrominoIdentifier tetrominoBag[TETROMINO_COUNT] = {I, O, T, Z, S, L, J};

    if (dropCount >= TETROMINO_COUNT)
    {
        dropCount = 0;
        Shuffle(tetrominoBag, TETROMINO_COUNT);
    }

    const TetrominoShape* tetromino = GetTetrominoShapeByIdentifier(tetrominoBag[dropCount]);
    dropCount++;

    return tetromino;
}

void RotateDroppingTetromino(DroppingTetromino* droppingTetromino, const int rotationAmount)
{
    // TODO Is the following the most efficient way to do this? (No, maybe just use If statements to handle the negatives)
    const enum Orientation newDirection = (((droppingTetromino->rotation + rotationAmount) % 4) + 4) % 4;
    // Cant do negative modulo operations in C
    droppingTetromino->rotation = newDirection;
}


static void Shuffle(int* array, const size_t n)
{
    if (n > 1)
    {
        for (size_t i = 0; i < n - 1; i++)
        {
            // Pick a random index from i to n-1
            const size_t j = i + SDL_rand(n - i);

            // Swap array[i] and array[j]
            const int t = array[j];
            array[j] = array[i];
            array[i] = t;
        }
    }
}
