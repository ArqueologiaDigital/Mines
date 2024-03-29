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
            set_tile(x, y, cell & HASBOMB ? FLAG : EMPTY_FLAG);
            return;
        }
        if (cell & HASBOMB) {
            set_tile(x, y, cell & ISOPEN ? EXPLOSION : BOMB);
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

    set_tile(x, y, tile);
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

    for (uint8_t x = 0; x <= mf->width; x++) {
        for (uint8_t y = 0; y <= mf->height; y++) {
            uint8_t xx = MINEFIELD_X_OFFSET + x * 2;
            uint8_t yy = MINEFIELD_Y_OFFSET + y * 2 - 1;
            if (y > 0 && y <= mf->height) {
                if (x == 0) {
                    set_tile(xx, yy, MINEFIELD_VERTICAL_LEFT);
                } else if (x < mf->width) {
                    set_tile(xx, yy, MINEFIELD_VERTICAL_MIDDLE);
                } else {
                    set_tile(xx, yy, MINEFIELD_VERTICAL_RIGHT);
                }
            }

            if (x < mf->width - 1 && y < mf->height - 1)
                set_tile(xx + 2, yy + 3, MINEFIELD_CROSS);

            if (x > 0 && x <= mf->width) {
                xx--;
                yy++;
                if (y == 0) {
                    set_tile(xx, yy, MINEFIELD_HORIZONTAL_TOP);
                } else if (y < mf->height) {
                    set_tile(xx, yy, MINEFIELD_HORIZONTAL_MIDDLE);
                } else {
                    set_tile(xx, yy, MINEFIELD_HORIZONTAL_BOTTOM);
                }
            }
        }
    }

    set_tile(MINEFIELD_X_OFFSET, MINEFIELD_Y_OFFSET, MINEFIELD_CORNER_TOP_LEFT);

    set_tile(MINEFIELD_X_OFFSET + mf->width * 2, MINEFIELD_Y_OFFSET,
             MINEFIELD_CORNER_TOP_RIGHT);

    set_tile(MINEFIELD_X_OFFSET, MINEFIELD_Y_OFFSET + mf->height * 2,
             MINEFIELD_CORNER_BOTTOM_LEFT);

    set_tile(MINEFIELD_X_OFFSET + mf->width * 2,
             MINEFIELD_Y_OFFSET + mf->height * 2,
             MINEFIELD_CORNER_BOTTOM_RIGHT);

    for (uint8_t x = 0; x <= mf->width; x++) {
        if (x > 0 && x < mf->width) {
            set_tile(MINEFIELD_X_OFFSET + x * 2, MINEFIELD_Y_OFFSET,
                     MINEFIELD_TOP_TEE);

            set_tile(MINEFIELD_X_OFFSET + x * 2,
                     MINEFIELD_Y_OFFSET + mf->height * 2, MINEFIELD_BOTTOM_TEE);
        }
    }

    for (uint8_t y = 1; y <= mf->height; y++) {
        if (y < mf->height) {
            set_tile(MINEFIELD_X_OFFSET, MINEFIELD_Y_OFFSET + y * 2,
                     MINEFIELD_LEFT_TEE);

            set_tile(MINEFIELD_X_OFFSET + mf->width * 2,
                     MINEFIELD_Y_OFFSET + y * 2, MINEFIELD_RIGHT_TEE);
        }
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

    for (x = 1; x < SCREEN_WIDTH - 1; x++) {
        for (y = 1; y < SCREEN_HEIGHT; y++) {
            set_tile(x, y, GROUND);
        }
    }

    set_tile(0, 0, CORNER_TOP_LEFT);
    set_tile(SCREEN_WIDTH - 1, 0, CORNER_TOP_RIGHT);
    set_tile(0, SCREEN_HEIGHT - 1, CORNER_BOTTOM_LEFT);
    set_tile(SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1, CORNER_BOTTOM_RIGHT);

    for (y = 1; y < SCREEN_HEIGHT / 2; y++) {
        set_tile(0, y, LEFT_BORDER__TOP);
        set_tile(SCREEN_WIDTH - 1, y, RIGHT_BORDER__TOP);
    }

    for (y = SCREEN_HEIGHT / 2; y < SCREEN_HEIGHT - 1; y++) {
        set_tile(0, y, LEFT_BORDER__BOTTOM);
        set_tile(SCREEN_WIDTH - 1, y, RIGHT_BORDER__BOTTOM);
    }

    for (x = 1; x < SCREEN_WIDTH / 2; x++) {
        set_tile(x, 0, TOP_BORDER__LEFT);
        set_tile(x, SCREEN_HEIGHT - 1, BOTTOM_BORDER__LEFT);
    }

    for (x = SCREEN_WIDTH / 2; x < SCREEN_WIDTH - 1; x++) {
        set_tile(x, 0, TOP_BORDER__RIGHT);
        set_tile(x, SCREEN_HEIGHT - 1, BOTTOM_BORDER__RIGHT);
    }
#endif
}
