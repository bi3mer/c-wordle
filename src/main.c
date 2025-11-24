#include "centered_text.h"
#include "raylib.h"
#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "words.h"

#define NUM_GUESSES 6
#define WORD_LENGTH 5

#define CELL_SIZE 100
#define CELL_SPACING 8

typedef struct
{
    Color color;
    char c;
} Tile;

static inline bool compare_tile_to_string(const Tile tiles[WORD_LENGTH],
                                          const char word[WORD_LENGTH]);

int main(void)
{
    ///////////////////////////////////////////////////////////////////////////
    // Set up
    const int screen_width = 800;
    const int screen_height = 800;

    InitWindow(screen_width, screen_height, "Wordle");
    SetTargetFPS(60);

    SetRandomSeed(time(NULL));

    Centered_Text title;
    centered_text_init(&title, "Wordle Clone", 40, 30, WHITE, screen_width);

    // size_t word_index = GetRandomValue(0, NUM_WORDS - 1);
    size_t word_index = 0;
    int guess_index = 0;
    size_t guess_word_index = 0;

    Tile guesses[NUM_GUESSES][WORD_LENGTH];
    for (size_t jj = 0; jj < WORD_LENGTH; ++jj)
    {
        guesses[0][jj].color = RAYWHITE;
        guesses[0][jj].c = '\0';
    }

    for (size_t i = 1; i < NUM_GUESSES; ++i)
    {
        for (size_t jj = 0; jj < WORD_LENGTH; ++jj)
        {
            guesses[i][jj].color = BLACK;
            guesses[i][jj].c = '\0';
        }
    }

    int invalid_word_frame_count = -1;

    ///////////////////////////////////////////////////////////////////////////
    // Game Loop
    while (!WindowShouldClose())
    {
        //---------------------------------------------------------------------
        // Game Logic
        //---------------------------------------------------------------------
        int key = GetKeyPressed();
        while (key != 0)
        {
            if (isalpha(key) && guess_word_index < WORD_LENGTH)
            {
                guesses[guess_index][guess_word_index].c = (char)tolower(key);
                ++guess_word_index;
                guesses[guess_index][guess_word_index].c = '\0';
            }
            else if (key == KEY_BACKSPACE && guess_word_index > 0)
            {
                --guess_word_index;
                guesses[guess_index][guess_word_index].c = '\0';
            }
            else if (key == KEY_ENTER && guess_word_index >= WORD_LENGTH - 1)
            {
                size_t i;
                for (i = 0; i < NUM_WORDS; ++i)
                {
                    if (compare_tile_to_string(guesses[guess_index], WORDS[i]))
                        break;
                }

                if (i < NUM_WORDS)
                {
                    if (i == word_index)
                    {
                        for (size_t jj = 0; jj < WORD_LENGTH; ++jj)
                        {
                            guesses[guess_index][jj].color = GREEN;
                        }
                        // TODO: PLAYER WON
                    }
                    else
                    {
                        // figure out tile colors for the guess
                        for (size_t jj = 0; jj < WORD_LENGTH; ++jj)
                        {
                            if (WORDS[word_index][jj] ==
                                guesses[guess_index][jj].c)
                            {
                                guesses[guess_index][jj].color = GREEN;
                            }
                            else
                            {
                                guesses[guess_index][jj].color = GRAY;
                            }
                        }

                        ++guess_index;
                        for (size_t jj = 0; jj < WORD_LENGTH; ++jj)
                        {
                            guesses[guess_index][jj].color = RAYWHITE;
                        }

                        guess_word_index = 0;
                    }

                    // TODO: If the number of guesses is greater than the number
                    //       of rows, then the game is over and the player lost
                }
                else
                {
                    invalid_word_frame_count = 120;
                }
            }

            key = GetKeyPressed();
        }

        if (invalid_word_frame_count >= 0)
        {
            --invalid_word_frame_count;
        }

        //---------------------------------------------------------------------
        // Rendering
        //---------------------------------------------------------------------
        BeginDrawing();
        ClearBackground(BLACK);

        centered_text_render(&title);

        if (invalid_word_frame_count > 0)
        {
            Centered_Text ct;
            centered_text_init(&ct, (char *)"Invalid Word", 20, 80, RED,
                               screen_width);
            centered_text_render(&ct);
        }

        const int start_x = 130;
        const int start_y = 120;

        for (int row = 0; row < NUM_GUESSES; row++)
        {
            const int y = start_y + row * (CELL_SIZE + CELL_SPACING);

            for (int col = 0; col < WORD_LENGTH; col++)
            {
                const int x = start_x + col * (CELL_SIZE + CELL_SPACING);
                const Tile *t = &guesses[row][col];

                DrawRectangle(x, y, CELL_SIZE, CELL_SIZE, t->color);

                DrawRectangleLinesEx((Rectangle){x, y, CELL_SIZE, CELL_SIZE}, 2,
                                     WHITE);

                if (t->c != '\0')
                {
                    char buf[2];
                    buf[0] = t->c;
                    buf[1] = '\0';
                    DrawText(buf, x + 30, y + 30, 60, BLACK);
                }
            }
        }

        EndDrawing();
    }

    ///////////////////////////////////////////////////////////////////////////
    // Clean Up
    CloseWindow();
    return 0;
}

static inline bool compare_tile_to_string(const Tile tiles[WORD_LENGTH],
                                          const char word[WORD_LENGTH])
{
    bool res = true;
    for (size_t i = 0; i < WORD_LENGTH; ++i)
    {
        if (tiles[i].c != word[i])
        {
            res = false;
            break;
        }
    }

    return res;
}
