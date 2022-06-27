#include "common.h" 
#include "minefield.h"


void game_over(minefield* mf)
{
    mf->changed = true;
    mf->state = GAME_OVER;

    // show all bomb locations
    for (uint8_t x = 0; x < mf->width; x++) {
        for (uint8_t y = 0; y < mf->height; y++) {
            uint8_t cell = CELL(mf, x, y);
            if (!(cell & HASFLAG) && (cell & HASBOMB)) {
                CELL(mf, x, y) |= ISOPEN;
            }
        }
    }
}


#ifndef RESET_MINEFIELD
void fill_minefield(minefield* mf)
{
    // Clear the minefield:
    for (uint8_t x1 = 0; x1 < mf->width; x1++) {
        for (uint8_t y1 = 0; y1 < mf->height; y1++) {
            CELL(mf, x1, y1) = 0;
        }
    }

    // Place mines:
    uint8_t mines = mf->mines;
    while (mines--) {
        uint8_t x2, y2;

        do {
            x2 = random_number(0, mf->width - 1);
            y2 = random_number(0, mf->height - 1);
        } while (CELL(mf, x2, y2) & HASBOMB);

        CELL(mf, x2, y2) |= HASBOMB;
    }

    // Compute neighbouring bombs:
    for (uint8_t x = 0; x < mf->width; x++) {
        for (uint8_t y = 0; y < mf->height; y++) {
            if ((CELL(mf, x, y) & HASBOMB)) continue;

            uint8_t count = 0;
            if (x > 0) {
                if (y > 0 && (CELL(mf, x - 1, y - 1) & HASBOMB)) count++;
                if ((CELL(mf, x - 1, y) & HASBOMB)) count++;
                if (y < mf->height - 1 && (CELL(mf, x - 1, y + 1) & HASBOMB)) count++;
            }

            if (y > 0 && (CELL(mf, x, y - 1) & HASBOMB)) count++;
            if (y < mf->height - 1 && (CELL(mf, x, y + 1) & HASBOMB)) count++;

            if (x < mf->width - 1) {
                if (y > 0 && (CELL(mf, x + 1, y - 1) & HASBOMB)) count++;
                if ((CELL(mf, x + 1, y) & HASBOMB)) count++;
                if (y < mf->height - 1 && (CELL(mf, x + 1, y + 1) & HASBOMB)) count++;
            }

            // Store the count in the 4 lowest bits:
            CELL(mf, x, y) |= count;
        }
    }

    mf->changed = false;
}
#endif /* RESET_MINEFIELD */


#ifndef MINEFIELD_ALLOCATION
minefield* init_minefield()
{
    minefield* mf = calloc(1, sizeof(minefield));
    mf->width = 10;
    mf->height = 10;
    mf->cells = calloc(mf->width, mf->height);

    return mf;
}


void free_minefield(minefield* mf)
{
    free(mf->cells);
    free(mf);
}
#endif /* MINEFIELD_ALLOCATION */


#ifndef RESET_MINEFIELD
void reset_minefield(minefield* mf)
{
    mf->mines = random_number(10, 30);
    debug("num_bombs = ", mf->mines);
    fill_minefield(mf);
}
#endif /* RESET_MINEFIELD */


void open_cell(minefield* mf, uint8_t x, uint8_t y)
{
    if (CELL(mf, x, y) & (HASQUESTIONMARK | HASFLAG | ISOPEN)) {
        return;
    } else {
        mf->changed = true;
        CELL(mf, x, y) |= ISOPEN;

        if (CELL(mf, x, y) & HASBOMB) {
            game_over(mf);
            return;
        }
    }

    if ((CELL(mf, x, y) & 0x0F) > 0)
        return;

    if (x - 1 >= 0) {
        if (y - 1 >= 0) open_cell(mf, x - 1, y - 1); // top-left
        if (y + 1 < mf->height) open_cell(mf, x - 1, y + 1); // bottom-left
        open_cell(mf, x - 1, y); // left
    }

    if (y - 1 >= 0) open_cell(mf, x, y - 1); // top

    if (x + 1 < mf->width) {
        if (y - 1 >= 0) open_cell(mf, x + 1, y - 1); // top-right
        if (y + 1 < mf->height) open_cell(mf, x + 1, y + 1); // bottom-right
        open_cell(mf, x + 1, y); // right
    }

    if (y + 1 < mf->height) open_cell(mf, x, y + 1); // bottom
}


void open_block(minefield* mf, uint8_t x, uint8_t y)
{
    if (x - 1 >= 0) {
        if (y - 1 >= 0) open_cell(mf, x - 1, y - 1); // top-left
        if (y + 1 < mf->height) open_cell(mf, x - 1, y + 1); // top-right
        open_cell(mf, x - 1, y); // top
    }

    if (y - 1 >= 0) open_cell(mf, x, y - 1); // left
    open_cell(mf, x, y); // center
    if (y + 1 < mf->height) open_cell(mf, x, y + 1); // right

    if (x + 1 < mf->width) {
        if (y - 1 >= 0) open_cell(mf, x + 1, y - 1); // bottom-left
        if (y + 1 < mf->height) open_cell(mf, x + 1, y + 1); // bottom-right
        open_cell(mf, x + 1, y); // bottom
    }
}


uint8_t count_surrounding_flags(minefield* mf, uint8_t x, uint8_t y)
{
    uint8_t count = 0;

    if (x - 1 >= 0) {
        if (y - 1 >= 0 && (CELL(mf, x - 1, y - 1) & HASFLAG)) count++; // top-left
        if ((CELL(mf, x - 1, y) & HASFLAG)) count++; // top
        if (y + 1 < mf->height && (CELL(mf, x - 1, y + 1) & HASFLAG)) count++; // top-right
    }

    if (y - 1 >= 0 && (CELL(mf, x, y - 1) & HASFLAG)) count++; // left
    if (y + 1 < mf->height && (CELL(mf, x, y + 1) & HASFLAG)) count++; // right

    if (x + 1 < mf->width) {
        if (y - 1 >= 0 && (CELL(mf, x + 1, y - 1) & HASFLAG)) count++; // bottom-left
        if ((CELL(mf, x + 1, y) & HASFLAG)) count++; // bottom
        if (y + 1 < mf->height && (CELL(mf, x + 1, y + 1) & HASFLAG)) count++; // bottom-right
    }

    return count;
}


void maybe_game_won(minefield* mf)
{
    for (uint8_t x = 0; x < mf->width; x++) {
        for (uint8_t y = 0; y < mf->height; y++) {
            uint8_t cell = CELL(mf, x, y);

            if (!(cell & ISOPEN) && (!(cell & HASFLAG) || !(cell & HASBOMB))) {
                return;
            }
        }
    }

    mf->state = GAME_WON;
}
