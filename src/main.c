#include "raylib.h"
#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "centered_text.h"
#include "words.h"

#define NUM_GUESSES 6
#define WORD_LENGTH 6

#define CELL_SIZE 100
#define CELL_SPACING 8

#define NOTIFICATION_FRAME_COUNT 120

typedef struct
{
    Color color;
    char c;
} Tile;

static inline bool compare_tile_to_string(const Tile tiles[WORD_LENGTH],
                                          const char word[WORD_LENGTH]);

static inline void reset_guesses(Tile guesses[NUM_GUESSES][WORD_LENGTH]);

int main(void)
{
    ///////////////////////////////////////////////////////////////////////////
    // Set up
    const int screen_width = 800;
    const int screen_height = 800;

    InitWindow(screen_width, screen_height, "Wordle");
    SetTargetFPS(60);

    SetRandomSeed((unsigned int)time(NULL));

    Centered_Text title;
    centered_text_init(&title, "Wordle Clone", 40, 30, WHITE, screen_width);

    size_t word_index = (size_t)GetRandomValue(0, NUM_WORDS - 1);
    int guess_index = 0;
    size_t guess_word_index = 0;

    Tile guesses[NUM_GUESSES][WORD_LENGTH];
    reset_guesses(guesses);

    bool bad_letters[26] = {0};

    int invalid_word_frame_count = -1;
    int player_won_frame_count = -1;
    int player_lost_frame_count = -1;

    ///////////////////////////////////////////////////////////////////////////
    // Game Loop
    while (!WindowShouldClose())
    {
        //---------------------------------------------------------------------
        // Game Logic
        //---------------------------------------------------------------------
        int key = GetKeyPressed();
        while (key != 0 && player_won_frame_count == -1 &&
               player_lost_frame_count == -1)
        {
            if (isalpha(key) && guess_word_index < WORD_LENGTH - 1)
            {
                const char c = (char)tolower(key);

                guesses[guess_index][guess_word_index].color =
                    bad_letters[c - 'a'] ? GRAY : RAYWHITE;
                guesses[guess_index][guess_word_index].c = c;
                ++guess_word_index;
                guesses[guess_index][guess_word_index].c = '\0';
            }
            else if (key == KEY_BACKSPACE && guess_word_index > 0)
            {
                --guess_word_index;
                guesses[guess_index][guess_word_index].color = RAYWHITE;
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

                        player_won_frame_count = NOTIFICATION_FRAME_COUNT;
                    }
                    else
                    {
                        //// figure out tile colors for the guess
                        int counts[26] = {0};
                        size_t jj;

                        // get counts of each letter in the word
                        for (jj = 0; jj < WORD_LENGTH - 1; ++jj)
                        {
                            ++counts[WORDS[word_index][jj] - 'a'];
                        }

                        // prioritize finding green letters first
                        for (jj = 0; jj < WORD_LENGTH - 1; ++jj)
                        {
                            char c = WORDS[word_index][jj];

                            if (c == guesses[guess_index][jj].c)
                            {
                                guesses[guess_index][jj].color = GREEN;
                                --counts[c - 'a'];
                            }
                        }

                        // find yellow letters next, and if they aren't in the
                        // counts then make them gray
                        for (jj = 0; jj < WORD_LENGTH - 1; ++jj)
                        {
                            if (guesses[guess_index][jj].color.g != GREEN.g)
                            {
                                char c = guesses[guess_index][jj].c;

                                if (counts[c - 'a'] > 0)
                                {
                                    guesses[guess_index][jj].color = YELLOW;
                                    --counts[c - 'a'];
                                }
                                else
                                {
                                    guesses[guess_index][jj].color = GRAY;
                                    bad_letters[c - 'a'] = true;
                                }
                            }
                        }

                        ++guess_index;
                        if (guess_index < NUM_GUESSES)
                        {
                            for (jj = 0; jj < WORD_LENGTH - 1; ++jj)
                            {
                                guesses[guess_index][jj].color = RAYWHITE;
                            }
                        }

                        guess_word_index = 0;
                    }

                    if (guess_index >= NUM_GUESSES &&
                        player_won_frame_count == -1)
                    {
                        player_lost_frame_count =
                            (int)(NOTIFICATION_FRAME_COUNT * 1.5f);
                    }
                }
                else
                {
                    invalid_word_frame_count = NOTIFICATION_FRAME_COUNT;
                }
            }

            key = GetKeyPressed();
        }

        if (invalid_word_frame_count >= 0)
            --invalid_word_frame_count;

        if (player_won_frame_count >= 0)
        {
            --player_won_frame_count;
            if (player_won_frame_count == 0)
            {
                word_index = (size_t)GetRandomValue(0, NUM_WORDS - 1);
                guess_index = 0;
                guess_word_index = 0;
                reset_guesses(guesses);

                invalid_word_frame_count = -1;
                player_won_frame_count = -1;
                player_lost_frame_count = -1;

                memset(bad_letters, 0, sizeof(bad_letters));
            }
        }
        if (player_lost_frame_count >= 0)
        {
            --player_lost_frame_count;
            if (player_lost_frame_count == 0)
            {
                word_index = (size_t)GetRandomValue(0, NUM_WORDS - 1);
                guess_index = 0;
                guess_word_index = 0;
                reset_guesses(guesses);

                invalid_word_frame_count = -1;
                player_won_frame_count = -1;
                player_lost_frame_count = -1;

                memset(bad_letters, 0, sizeof(bad_letters));
            }
        }

        //---------------------------------------------------------------------
        // Rendering
        //---------------------------------------------------------------------
        BeginDrawing();
        ClearBackground(BLACK);

        centered_text_render(&title);

        if (player_won_frame_count > 0)
        {
            Centered_Text ct;
            centered_text_init(&ct, (char *)"You Won!!!", 20, 80, GREEN,
                               screen_width);
            centered_text_render(&ct);
        }
        else if (player_lost_frame_count > 0)
        {
            char buffer[24];
            sprintf(buffer, "Correct Word was: %s", WORDS[word_index]);
            Centered_Text ct;
            centered_text_init(&ct, buffer, 20, 80, RED, screen_width);
            centered_text_render(&ct);
        }
        else if (invalid_word_frame_count > 0)
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

            for (int col = 0; col < WORD_LENGTH - 1; col++)
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

static inline void reset_guesses(Tile guesses[NUM_GUESSES][WORD_LENGTH])
{
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
}
