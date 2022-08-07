#include <string.h>
#include <stdbool.h>
#include "msx2.h"
#include "common.h"
#include "game.h"
#include "minefield.h"
#include "tiles.h"
#include "timer.h"

//#define USE_DEBUG_MODE
#include "debug.h"

/* Assets */
#include "build/mines.h"
#include "cursors.h"
#include "tile_coords.c"

typedef void (*Callback)(void);


// mine control
bool negative = false;
uint8_t units = 0; /* makeshift BCD counter */
uint8_t decimals = 0; /* makeshift BCD counter */


void vblank_hook()
{
    count_bcd_time();
    // TODO: play a song
}


void video_init()
{
    disable_screen();

    set_colors(15, 1, 1);

    /* Setting SCREEN5, 16x16 sprites, display disabled, vblank enabled */
    write_vdp(0, 6);
    write_vdp(1, 0x62);

    write_vdp(2, 0x1f); /* Setting Pattern name table to 0x0 */

    /* Setting sprite attribute table to 0x7600 */
    write_vdp(5, 0xef);
    write_vdp(11, 0);  

    write_vdp(6, 0x0f); /* Setting sprite pattern generator table to 0x7800 */

    write_vdp(9, 0x80); /* 212 lines */

    /* blank 32k on first page */
    fill_vram(0x00, 32768, 0x0);

    //restore_palette();
    set_palette(15, mines_palette);

    /* Move board and mines from RAM to second VRAM page */
    write_vram(0x8000, mines_SIZE, mines_data);

    /* Set cursor sprite */
    set_sprite_pattern(cursor_pattern, CURSOR_PATTERN_ID);
    put_sprite_colors(cursor_colors, CURSOR_SPRITE_ID);
    hide_cursor();

    /* Set mouse sprite */
    set_sprite_pattern(mouse_pattern, MOUSE_PATTERN_ID);
    put_sprite_colors(mouse_colors, MOUSE_SPRITE_ID);
    hide_mouse();

    /* Set 60Hz hook */
    __asm__("di");
    *((uint8_t*) HTIMI) = 0xc3;                 // jump opcode
    *((Callback*) (HTIMI + 1)) = vblank_hook;   // + address
    __asm__("ei");

    enable_screen();
}


/* set_tile emulates tile behaviour, but is actually a bitmap copy */
void set_tile(uint8_t dst_x, uint8_t dst_y, uint8_t tile)
{
    /* copy 8x8 block from page 1 (hidden page) to page 0 (visible page) */
    vdp(TILE_X[tile], TILE_Y[tile], dst_x * 8, dst_y * 8, 8, 8, DIR_DEFAULT, VDP_HMMM);
}


/* set_block copies block of 16x16 tiles to x, y coordinates */
void set_block(uint8_t dst_x, uint8_t dst_y, uint8_t tile)
{
    /* copy 16x16 block from page 1 (hidden page) to page 0 (visible page) */
    vdp(TILE_X[tile], TILE_Y[tile], dst_x * 8, dst_y * 8, 16, 16, DIR_DEFAULT, VDP_HMMM);
}


void put_cursor(uint8_t x, uint8_t y)
{
    struct sprite_attr attr = { y - 1, x, 4 * CURSOR_PATTERN_ID, 0 };
    put_sprite_attr(&attr, CURSOR_SPRITE_ID);
}


void hide_cursor()
{
    put_cursor(0, 209);
}


void put_mouse(uint8_t x, uint8_t y)
{
    struct sprite_attr attr = { y - 1, x, 4 * MOUSE_PATTERN_ID, 0 };
    put_sprite_attr(&attr, MOUSE_SPRITE_ID);
}


void hide_mouse()
{
    put_mouse(0, 209);
}


void highlight_current_cell(minefield* mf)
{
    uint8_t x = CELL_X(mf, mf->current_cell) * 2 + MINEFIELD_X_OFFSET + 1;
    uint8_t y = CELL_Y(mf, mf->current_cell) * 2 + MINEFIELD_Y_OFFSET + 1;

    if (mf->state == PLAYING_GAME) {
        put_cursor(x * 8 + CURSOR_X_OFFSET, y * 8 + CURSOR_Y_OFFSET);
    } else {
        hide_cursor();
    }
}


void platform_init()
{
    set_random_seed(read_clock());
    video_init();
}


void draw_timer()
{
    set_tile(HOURGLASS_X_POS - 1, HOURGLASS_Y_POS - 1, FRAME_TOP_LEFT);
    set_tile(HOURGLASS_X_POS + 0, HOURGLASS_Y_POS - 1, FRAME_TOP_CENTER);
    set_tile(HOURGLASS_X_POS + 1, HOURGLASS_Y_POS - 1, FRAME_TOP_CENTER);
    set_tile(HOURGLASS_X_POS + 2, HOURGLASS_Y_POS - 1, FRAME_TOP_CENTER);
    set_tile(HOURGLASS_X_POS + 3, HOURGLASS_Y_POS - 1, FRAME_TOP_CENTER);
    set_tile(HOURGLASS_X_POS + 4, HOURGLASS_Y_POS - 1, FRAME_TOP_CENTER);
    set_tile(HOURGLASS_X_POS + 5, HOURGLASS_Y_POS - 1, FRAME_TOP_CENTER);
    set_tile(HOURGLASS_X_POS + 6, HOURGLASS_Y_POS - 1, FRAME_TOP_RIGHT);

    set_tile(HOURGLASS_X_POS - 1, HOURGLASS_Y_POS, FRAME_VERTICAL_LEFT);
    set_tile(HOURGLASS_X_POS, HOURGLASS_Y_POS, HOURGLASS);
    set_tile(HOURGLASS_X_POS + 6, HOURGLASS_Y_POS, FRAME_VERTICAL_RIGHT);

    set_tile(HOURGLASS_X_POS - 1, HOURGLASS_Y_POS + 1, FRAME_BOTTOM_LEFT);
    set_tile(HOURGLASS_X_POS + 0, HOURGLASS_Y_POS + 1, FRAME_BOTTOM_CENTER);
    set_tile(HOURGLASS_X_POS + 1, HOURGLASS_Y_POS + 1, FRAME_BOTTOM_CENTER);
    set_tile(HOURGLASS_X_POS + 2, HOURGLASS_Y_POS + 1, FRAME_BOTTOM_CENTER);
    set_tile(HOURGLASS_X_POS + 3, HOURGLASS_Y_POS + 1, FRAME_BOTTOM_CENTER);
    set_tile(HOURGLASS_X_POS + 4, HOURGLASS_Y_POS + 1, FRAME_BOTTOM_CENTER);
    set_tile(HOURGLASS_X_POS + 5, HOURGLASS_Y_POS + 1, FRAME_BOTTOM_CENTER);
    set_tile(HOURGLASS_X_POS + 6, HOURGLASS_Y_POS + 1, FRAME_BOTTOM_RIGHT);
}


inline void count_bcd_mines(const minefield* mf)
{
    negative = mf->mines < 0;
    int8_t tmp = negative ? -mf->mines : mf->mines;

    units = tmp % 10;
    decimals = tmp / 10;
}


void draw_counter()
{
    set_tile(BOMB_ICON_X_POS - 1, BOMB_ICON_Y_POS - 1, FRAME_TOP_LEFT);
    set_tile(BOMB_ICON_X_POS + 0, BOMB_ICON_Y_POS - 1, FRAME_TOP_CENTER);
    set_tile(BOMB_ICON_X_POS + 1, BOMB_ICON_Y_POS - 1, FRAME_TOP_CENTER);
    set_tile(BOMB_ICON_X_POS + 2, BOMB_ICON_Y_POS - 1, FRAME_TOP_CENTER);
    set_tile(BOMB_ICON_X_POS + 3, BOMB_ICON_Y_POS - 1, FRAME_TOP_CENTER);
    set_tile(BOMB_ICON_X_POS + 4, BOMB_ICON_Y_POS - 1, FRAME_TOP_CENTER);
    set_tile(BOMB_ICON_X_POS + 5, BOMB_ICON_Y_POS - 1, FRAME_TOP_CENTER);
    set_tile(BOMB_ICON_X_POS + 6, BOMB_ICON_Y_POS - 1, FRAME_TOP_RIGHT);

    set_tile(BOMB_ICON_X_POS - 1, BOMB_ICON_Y_POS, FRAME_VERTICAL_LEFT);
    set_tile(BOMB_ICON_X_POS, BOMB_ICON_Y_POS, BOMB_ICON);
    set_tile(BOMB_ICON_X_POS + 1, BOMB_ICON_Y_POS, NO_SIGN);
    set_tile(BOMB_ICON_X_POS + 2, BOMB_ICON_Y_POS, NO_SIGN);
    set_tile(BOMB_ICON_X_POS + 6, BOMB_ICON_Y_POS, FRAME_VERTICAL_RIGHT);

    set_tile(BOMB_ICON_X_POS - 1, BOMB_ICON_Y_POS + 1, FRAME_BOTTOM_LEFT);
    set_tile(BOMB_ICON_X_POS + 0, BOMB_ICON_Y_POS + 1, FRAME_BOTTOM_CENTER);
    set_tile(BOMB_ICON_X_POS + 1, BOMB_ICON_Y_POS + 1, FRAME_BOTTOM_CENTER);
    set_tile(BOMB_ICON_X_POS + 2, BOMB_ICON_Y_POS + 1, FRAME_BOTTOM_CENTER);
    set_tile(BOMB_ICON_X_POS + 3, BOMB_ICON_Y_POS + 1, FRAME_BOTTOM_CENTER);
    set_tile(BOMB_ICON_X_POS + 4, BOMB_ICON_Y_POS + 1, FRAME_BOTTOM_CENTER);
    set_tile(BOMB_ICON_X_POS + 5, BOMB_ICON_Y_POS + 1, FRAME_BOTTOM_CENTER);
    set_tile(BOMB_ICON_X_POS + 6, BOMB_ICON_Y_POS + 1, FRAME_BOTTOM_RIGHT);
}


void update_counter(minefield* mf)
{
    count_bcd_mines(mf);
    set_tile(BOMB_ICON_X_POS + 3, BOMB_ICON_Y_POS, negative ? MINUS_SIGN : NO_SIGN);
    set_tile(BOMB_ICON_X_POS + 4, BOMB_ICON_Y_POS, ZERO_DIGIT + decimals);
    set_tile(BOMB_ICON_X_POS + 5, BOMB_ICON_Y_POS, ZERO_DIGIT + units);
}


extern inline void update_mouse(minefield* mf, uint8_t source);


void idle_update(minefield* mf)
{
    static uint8_t ignore_input = 0;
    static uint8_t mouse1 = 0;
    static uint8_t mouse2 = 0;
    static uint8_t source = 0xff;
    static uint8_t fifth = 0;

    switch (++fifth) {
        case 4: {
            /* count how many times mouse is not found */
            int8_t mouse = search_mouse();
            switch (mouse) {
            case -1:
                ignore_input++;
                break;
            case 1:
                ignore_input = 0;
                if (source == 0xff && mouse1 > IGNORE_MOUSE_THRESHOLD) {
                    debug_msg("mouse1 detected\n");
                    mouse2 = 0;
                    source = 1;
                }
                mouse1++;
                break;
            case 2:
                ignore_input = 0;
                if (source == 0xff && mouse2 > IGNORE_MOUSE_THRESHOLD) {
                    debug_msg("mouse2 detected\n");
                    mouse1 = 0;
                    source = 2;
                }
                mouse2++;
                break;
            default:
                ignore_input = 0;
                break;
            }
        }
        case 5:
            fifth = 0;
            if (ignore_input > IGNORE_MOUSE_THRESHOLD) {
                hide_mouse();
                source = 0xff;
            } else if (source != 0xff) {
                update_mouse(mf, source);
            }
            break;
        default:
            break;
    }
}


void platform_shutdown()
{
    // Cartridge games can't unload.
    __asm__("jp 0");
}
