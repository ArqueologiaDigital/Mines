#include "../../common/game.h"
#include <psxsio.h>
#include <stdio.h>

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

void debug(char* msg, uint8_t value) {
    printf("%s%d\n", msg, value);
}

void platform_init() {
    int counter;

    // Init stuff   
    init_gl();
    start_pad();

    #ifdef USE_DEBUG_MODE
    AddSIO(115200);
    printf("Using debug mode...\n");
    #endif
}

// We don't need to implement that :)
void platform_shutdown() {}
