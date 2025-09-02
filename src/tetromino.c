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

    if (identifier <= 0 || (int)identifier > TETROMINO_COUNT)
    {
        return NULL;
    }

    // Note: The order of this array must match the TetrominoIdentifier enum
    TetrominoShape* tetrominoes[TETROMINO_COUNT] = {&pieceI, &pieceO, &pieceT, &pieceZ, &pieceS, &pieceL, &pieceJ};
    return tetrominoes[identifier - 1]; // Identifiers are 1-indexed, array is 0-indexed
}

void InitTetrominoBag(TetrominoBag* bag)
{
    bag->dropCount = 0;
    for (int i = 0; i < TETROMINO_COUNT; i++) {
        bag->bag[i] = (TetrominoIdentifier)(i + 1);
    }
    Shuffle(bag->bag, TETROMINO_COUNT);
}

const TetrominoShape* NextTetrominoFromBag(TetrominoBag* bag)
{
    if (bag->dropCount >= TETROMINO_COUNT) {
        InitTetrominoBag(bag); // reshuffle
    }

    const TetrominoShape* tetromino = GetTetrominoShapeByIdentifier(bag->bag[bag->dropCount]);

    bag->dropCount++;
    return tetromino;
}

void RotateDroppingTetromino(DroppingTetromino* droppingTetromino, const int rotationAmount)
{
    // NOTE: & 3 Does the same as wrapping 0-3, but makes for cleaner code as rotationAmount can be negative
    // and in C, you can't easily use modulus to wrap negatives. This trick only works when % is a power of two.
    droppingTetromino->orientation = (droppingTetromino->orientation + rotationAmount) & 3;
}

void Shuffle(int* array, const size_t n)
{
    // Fisher-Yates Shuffle
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