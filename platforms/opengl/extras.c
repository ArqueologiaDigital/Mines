#include <GL/glut.h>
#include "common.h"
#include "game.h"
#include "minefield.h"

/* platform code can use game-defined functions */
#include "main.h"


static minefield* mf;


minefield* init_minefield()
{
    mf = calloc(1, sizeof(minefield));
    mf->width = 10;
    mf->height = 10;
    mf->cells = calloc(mf->width * mf->height, sizeof(uint8_t));

    return mf;
}


void update_title_screen()
{
    mf->state = PLAYING_GAME;
    reset_minefield(mf);
    draw_minefield(mf);
}


void update_game_over()
{
    draw_minefield_contents(mf);
    draw_minefield(mf);

    uint8_t input = input_read(KEYBOARD);
    if (input == MINE_INPUT_IGNORED)
        return;

    switch (input) {
        case MINE_INPUT_OPEN:
        case MINE_INPUT_OPEN_BLOCK:
        case MINE_INPUT_FLAG:
            mf->state = TITLE_SCREEN;
            break;

        case MINE_INPUT_QUIT:
            mf->state = QUIT;
            break;

        default:
            /* Ignore cursor moves when game over */
            break;
    }
}


void update_gameplay()
{
    draw_minefield_contents(mf);
    draw_minefield(mf);

    uint8_t input = input_read(KEYBOARD);

    if (input == MINE_INPUT_IGNORED) {
        return;
    }

    update_gameplay_input(mf, input);
}


void update_main_loop()
{
    switch (mf->state)
    {
        case TITLE_SCREEN:
            update_title_screen();
            break;
        case PLAYING_GAME:
            update_gameplay();
            break;
        case GAME_OVER:
            update_game_over();
            break;
    }
}


void platform_main_loop(minefield* mf)
{
    UNUSED(mf);
    glutMainLoop();
}


void free_minefield(minefield* mf)
{
    free(mf->cells);
    free(mf);
}
