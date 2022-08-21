#include "tiles.h"
#include "common.h"
#include "minefield.h"
#include "codes.h"


void draw_single_cell(minefield* mf, uint8_t x, uint8_t y)
{
    const uint8_t cell = CELL(mf, x, y);
    uint8_t tile = CLOSED_CELL;

    x = x * 2 + MINEFIELD_X_OFFSET + 1;
    y = y * 2 + MINEFIELD_Y_OFFSET + 1;

    if (mf->state != PLAYING_GAME) {
        if (cell & HASFLAG) {
            set_block(x, y, cell & HASBOMB ? FLAG : EMPTY_FLAG);
            return;
        }
        if (cell & HASBOMB) {
            set_block(x, y, cell & ISOPEN ? EXPLOSION : BOMB);
            return;
        }
    }

    if (cell & ISOPEN) {
        uint8_t count = cell & 0x0F;
        if (count > 0 && count < 9)
            tile = ONE_BOMB + count - 1;
        else
            tile = BLANK;
    } else if (cell & HASFLAG)
        tile = FLAG;
    else if (cell & HASQUESTIONMARK)
        tile = QUESTION_MARK;

    set_block(x, y, tile);
}


void draw_minefield_contents(minefield* mf)
{
    if (mf->changed) {
        mf->changed = false;
        for (uint8_t x = 0; x < mf->width; x++) {
            for (uint8_t y = 0; y < mf->height; y++) {
                draw_single_cell(mf, x, y);
            }
        }
    }

    highlight_current_cell(mf);
}


void draw_minefield(minefield* mf)
{
#ifdef ENABLE_PLATFORM_DRAW_HOOK
    platform_begin_draw();
#endif /* ENABLE_PLATFORM_DRAW_HOOK */

    uint8_t right_x = MINEFIELD_X_OFFSET + mf->width * 2 + 1;
    uint8_t bottom_y = MINEFIELD_Y_OFFSET + mf->height * 2 + 1;

    set_tile(MINEFIELD_X_OFFSET, MINEFIELD_Y_OFFSET, MINEFIELD_CORNER_TOP_LEFT);

    set_tile(MINEFIELD_X_OFFSET + mf->width * 2 + 1, MINEFIELD_Y_OFFSET,
             MINEFIELD_CORNER_TOP_RIGHT);

    set_tile(MINEFIELD_X_OFFSET, MINEFIELD_Y_OFFSET + mf->height * 2 + 1,
             MINEFIELD_CORNER_BOTTOM_LEFT);

    set_tile(MINEFIELD_X_OFFSET + mf->width * 2 + 1,
             MINEFIELD_Y_OFFSET + mf->height * 2 + 1,
             MINEFIELD_CORNER_BOTTOM_RIGHT);

    // draw minefield horizontal borders
    for (int x = 1; x < mf->width * 2 + 1; x++) {
        set_tile(MINEFIELD_X_OFFSET + x, MINEFIELD_Y_OFFSET, MINEFIELD_HORIZONTAL_TOP);
        set_tile(MINEFIELD_X_OFFSET + x, bottom_y, MINEFIELD_HORIZONTAL_BOTTOM);
    }

    // draw minefield vertical borders
    for (int y = 1; y < mf->height * 2 + 1; y++) {
        set_tile(MINEFIELD_X_OFFSET, MINEFIELD_Y_OFFSET + y, MINEFIELD_VERTICAL_LEFT);
        set_tile(right_x, MINEFIELD_Y_OFFSET + y, MINEFIELD_VERTICAL_RIGHT);
    }

    mf->changed = true;
    draw_minefield_contents(mf);

#ifdef ENABLE_PLATFORM_DRAW_HOOK
    platform_end_draw();
#endif /* ENABLE_PLATFORM_DRAW_HOOK */
}


void draw_scenario()
{
#if defined(SCREEN_WIDTH) && defined(SCREEN_HEIGHT)
    uint8_t x,y;

    // background
    for (x = 0; x < SCREEN_WIDTH; x += 2) {
        for (y = 0; y < SCREEN_HEIGHT; y += 2) {
            set_block(x, y, GROUND);
        }
    }
#endif
}
