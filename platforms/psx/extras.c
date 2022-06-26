#include "../../common/game.h"

minefield* init_minefield()
{
    // TODO - calloc seems to be crashing the emulator!
    //minefield* mf = calloc(1, sizeof(minefield));
    minefield* mf = malloc(1);
    mf->width = 10;
    mf->height = 10;
    //mf->cells = calloc(mf->width, mf->height);
    mf->cells = malloc(mf->width);

    return mf;
}


void free_minefield(minefield* mf)
{
    free(mf->cells);
    free(mf);
}

void platform_init() {
    int counter;

    // Init stuff   
    init_gl();
    start_pad();
}

