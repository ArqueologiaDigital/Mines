#include "../../common/game.h"

minefield* init_minefield()
{
    // TODO - understand why calloc doesn't work :(
    //minefield* mf = calloc(1, sizeof(minefield));
    minefield* mf = malloc(1);
    mf->width = 10;
    mf->height = 10;
    //mf->cells = calloc(mf->width, mf->height);
    mf->cells = malloc(1);

    return mf;
}


void free_minefield(minefield* mf)
{
    free(mf->cells);
    free(mf);
}
