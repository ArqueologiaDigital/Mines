#include "minefield.h"


static minefield* mf;


minefield* init_minefield()
{
    mf = calloc(1, sizeof(minefield));
    mf->width = 10;
    mf->height = 10;
    mf->cells = calloc(mf->width * mf->height, sizeof(uint8_t));

    return mf;
}


void gl_main_loop()
{
    switch (mf->state)
    {
        case TITLE_SCREEN:
            title_screen_loop(mf);
            break;
        case PLAYING_GAME:
            gameplay_loop(mf);
            break;
        case GAME_OVER:
            game_over_loop(mf);
            break;
    }
}


void free_minefield(minefield* mf)
{
    free(mf->cells);
    free(mf);
}
