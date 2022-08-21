#include <string.h>
#include <stdbool.h>
#include "msx2.h"
#include "common.h"
#include "tiles.h"
#include "game.h"
#include "minefield.h"


joydata mouse;

/* contextual mouse data */
static int x = 84;
static int y = 33;
static bool l_pressed = false;
static bool r_pressed = false;
static bool ignored = false;


void update_mouse(minefield* mf, uint8_t source)
{
    read_joyport(&mouse, source);

    x -= mouse.dx;
    if (x > 255) x = 255;
    else if (x < 0) x = 0;

    y -= mouse.dy;
    if (y > 212) y = 212;
    else if (y < 0) y = 0;

    put_mouse(x, y);

    if (mf->state == PLAYING_GAME) {
        uint8_t cell_x = x / 8 - MINEFIELD_X_OFFSET;
        uint8_t cell_y = y / 8 - MINEFIELD_Y_OFFSET;
#ifdef _8X8_TILES
        if (cell_x & 1 || cell_y & 1) return;
#endif // _8X8_TILES
        cell_x /= 2;
        cell_y /= 2;
        if (cell_x >= mf->width || cell_y >= mf->height) return;
        /* ignore presses again until both button1 and button2 are released */
        if (mouse.l_button + mouse.r_button == 5 && ignored) {
            ignored = false;
        } else if (ignored) {
            return;
        }

        /* was left button released? */
        if (mouse.l_button && l_pressed) {
            l_pressed = false;

            /* left button alone? */
            if (!r_pressed) {
                set_minefield_cell(mf, cell_x, cell_y, MINE_INPUT_OPEN);
            } else {
                /* or simultaneous clicks? */
                r_pressed = false;
                set_minefield_cell(mf, cell_x, cell_y, MINE_INPUT_OPEN_BLOCK);
                ignored = true;
            }
            return;
        } else if (!mouse.l_button) {
            /* remembers if left button was pressed */
            l_pressed = true;
        }
    
        /* was right button released? */
        if (mouse.r_button && r_pressed) {
            r_pressed = false;

            /* right button alone? */
            if (!l_pressed) {
                set_minefield_cell(mf, cell_x, cell_y, MINE_INPUT_FLAG);
            } else {
                /* or simultaneous clicks? */
                l_pressed = false;
                set_minefield_cell(mf, cell_x, cell_y, MINE_INPUT_OPEN_BLOCK);
                ignored = true;
            }
        } else if (!mouse.r_button) {
            /* remembers if right button was pressed */
            r_pressed = true;
        }
    }
}
