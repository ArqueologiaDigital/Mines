#include <stdint.h>
#include "minefield.h"

#define WIDTH 10
#define HEIGHT 10

static minefield _minefield;
static uint8_t _cells[WIDTH * HEIGHT];


minefield* init_minefield()
{
    minefield* mf = &_minefield;
    mf->state = TITLE_SCREEN;
    mf->width = WIDTH;
    mf->height = HEIGHT;
    mf->current_cell = 0;
    mf->cells = _cells;

    return mf;
}


void free_minefield(minefield* mf)
{
    mf;
}

int gettimeofday(){
    return 0; // TODO: Implement-me!
}

int times(){
    return 0; // TODO: Implement-me!
}
