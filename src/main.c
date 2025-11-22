#include "raylib.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#define WORDS_FILE "assets\\words.txt"
#else
#define WORDS_FILE "assets/words.txt"
#endif

#define NUM_WORDS 2316

int main(void)
{
    const int screen_width = 800;
    const int screen_height = 450;

    ///////////////////////////////////////////////////////////////////////////
    // Get words from the text file
    char words[NUM_WORDS][5];
    {
        size_t word_index = 0;
        FILE *f = fopen(WORDS_FILE, "r");
        if (!f)
        {
            fprintf(stderr, "ERROR: unable to open word text file: %s\n",
                    WORDS_FILE);
            exit(1);
        }

        char buffer[6];
        while (fgets(buffer, sizeof(buffer), f))
        {
            buffer[5] = '\0'; // skip new line
            strcpy(words[word_index], buffer);
            ++word_index;
        }

        fclose(f);
    }

    ///////////////////////////////////////////////////////////////////////////
    // Get Wordle Words
    InitWindow(screen_width, screen_height, "Wordle");
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawText("TEXT HERE", 190, 200, 20, BLACK);

        EndDrawing();
    }

    ///////////////////////////////////////////////////////////////////////////
    // Clean Up
    CloseWindow();

    for (size_t i = 0; i < NUM_WORDS; ++i)
    {
        printf("%s\n", words[i]);
        free(words[i]);
    }

    return 0;
}
