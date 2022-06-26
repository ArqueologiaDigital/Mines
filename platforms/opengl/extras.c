#include <GL/glut.h>
#include <GL/freeglut_ext.h>
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


void update_main_loop()
{
    switch (mf->state)
    {
        case TITLE_SCREEN:
            title_screen_update(mf);
            break;
        case PLAYING_GAME:
            draw_minefield_contents(mf);
            draw_minefield(mf);
            gameplay_update(mf);
            break;
        case GAME_WON:
        case GAME_OVER:
            draw_minefield_contents(mf);
            draw_minefield(mf);
            game_over_update(mf);
            break;
    case QUIT:
        glutLeaveMainLoop();
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
