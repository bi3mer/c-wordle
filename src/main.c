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
#define WORD_LENGTH 6 // 5 + 1 for '\0'

#define CELL_SIZE 100
#define CELL_SPACING 8

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
    centered_text_init(&title, (char *)"Wordle Clone", 40, 30, WHITE,
                       screen_width);

    // size_t word_index = GetRandomValue(0, NUM_WORDS - 1);
    size_t word_index = 0;
    int guess_index = 0;
    size_t guess_word_index = 0;

    char guesses[NUM_GUESSES][WORD_LENGTH];
    for (size_t i = 0; i < NUM_GUESSES; ++i)
    {
        for (size_t jj = 0; jj < WORD_LENGTH; ++jj)
        {
            guesses[i][jj] = '\0';
        }
    }

    ///////////////////////////////////////////////////////////////////////////
    // Game Loop
    while (!WindowShouldClose())
    {
        //---------------------------------------------------------------------
        // Game Logic
        //---------------------------------------------------------------------
        char key = (char)GetCharPressed();
        while (key != 0 && guess_word_index + 1 < WORD_LENGTH)
        {
            if (isalpha(key))
            {
                guesses[guess_index][guess_word_index] = (char)tolower(key);
                ++guess_word_index;
                guesses[guess_index][guess_word_index] = '\0';
            }

            key = (char)GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE) && guess_word_index > 0)
        {
            --guess_word_index;
            guesses[guess_index][guess_word_index] = '\0';
        }

        //---------------------------------------------------------------------
        // Rendering
        //---------------------------------------------------------------------
        BeginDrawing();
        ClearBackground(BLACK);

        centered_text_render(&title);

        const int start_x = 130;
        const int start_y = 120;

        for (int row = 0; row < NUM_GUESSES; row++)
        {
            for (int col = 0; col < WORD_LENGTH - 1; col++)
            {
                const int x = start_x + col * (CELL_SIZE + CELL_SPACING);
                const int y = start_y + row * (CELL_SIZE + CELL_SPACING);

                const char c = guesses[row][col];
                Color bgColor = BLACK;

                if (row < guess_index)
                {
                    if (c == WORDS[word_index][col])
                    {
                        bgColor = GREEN;
                    }
                    else if (c != '\0')
                    {
                        bgColor = RAYWHITE;
                    }
                }
                else if (row == guess_index)
                {
                    bgColor = RAYWHITE;
                }

                DrawRectangle(x, y, CELL_SIZE, CELL_SIZE, bgColor);

                DrawRectangleLinesEx((Rectangle){x, y, CELL_SIZE, CELL_SIZE}, 2,
                                     WHITE);

                if (c != '\0')
                {
                    char buf[2];
                    buf[0] = c;
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
