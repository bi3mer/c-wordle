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

int main(void)
{
    ///////////////////////////////////////////////////////////////////////////
    // Set up
    const int screen_width = 800;
    const int screen_height = 450;

    InitWindow(screen_width, screen_height, "Wordle");
    SetTargetFPS(60);

    SetRandomSeed(time(NULL));

    size_t word_index = GetRandomValue(0, NUM_WORDS - 1);
    size_t i;
    size_t guess_index = 0;
    size_t guess_word_index = 0;

    char guesses[NUM_GUESSES][WORD_LENGTH];
    for (i = 0; i < NUM_GUESSES; ++i)
    {
        guesses[i][0] = '\0';
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

        DrawText(guesses[0], 100, 100, 40, WHITE);

        EndDrawing();
    }

    ///////////////////////////////////////////////////////////////////////////
    // Clean Up
    CloseWindow();
    return 0;
}
