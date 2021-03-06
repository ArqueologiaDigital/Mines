#include <string.h>
#include <stdbool.h>
#include "msx2.h"
#include "common.h"
#include "video-tiles.h"
#include "minefield.h"

//#define USE_DEBUG_MODE
#include "debug.h"

/* Assets */
#include "mines.h"
#include "cursor.h"
#include "tile_coords.c"

typedef void (*Callback)(void);

/* Time controls */
uint8_t ticks = 0; /* add one tick each time HTIMI is called */
uint8_t seconds = 0; /* makeshift BCD counter */
uint8_t minutes = 0; /* makeshift BCD counter */

/* Mine control */
bool negative = false;
uint8_t units = 0; /* makeshift BCD counter */
uint8_t decimals = 0; /* makeshift BCD counter */


inline void count_bcd_time()
{
    /* Update timer */
    if (++ticks > 59) {
        ticks = 0; /* 60 ticks make a second at 60Hz */

        if ((++seconds & 0b1111) > 9) {
            seconds += 6; /* carry the BCD digit by adding 6 */

            if (seconds > 0x59) {
                seconds = 0;

                if ((++minutes & 0b1111) > 9)
                    minutes += 6; /* carry the BCD digit by adding 6 */

                if (minutes > 99)
                    minutes = 0;
            }
        }
    }
}


void vblank_hook()
{
    count_bcd_time();
    /* TODO: play song */
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
    set_sprite_pattern(cursor_pattern, PATTERN_ID);
    put_sprite_colors(cursor_colors, SPRITE_ID);
    hide_cursor();

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
    vdp(TILE_X[tile], TILE_Y[tile], dst_x * 8, dst_y * 8, 8, 8, DIR_DEFAULT, VDP_LMMM | PO_IMP);
}


void put_cursor(uint8_t x, uint8_t y)
{
    struct sprite_attr attr = { y - 1, x, 4 * PATTERN_ID, 0 };
    put_sprite_attr(&attr, SPRITE_ID);
}


void hide_cursor()
{
    put_cursor(0, 209);
}


void highlight_current_cell(minefield* mf)
{
    uint8_t x = CELL_X(mf, mf->current_cell) * 2 + MINEFIELD_X_OFFSET + 1;
    uint8_t y = CELL_Y(mf, mf->current_cell) * 2 + MINEFIELD_Y_OFFSET + 1;

    if (mf->state == PLAYING_GAME) {
        put_cursor(x * 8 - 3, y * 8 - 3);
    } else {
        hide_cursor();
    }
}


void platform_init()
{
    set_random_seed(read_clock());
    video_init();
}


void reset_timer()
{
    debug_msg("reset_timer() called\n");
    ticks = 0;
    seconds = 0;
    minutes = 0;
}


void update_timer(minefield* mf)
{
    mf;

    uint8_t unit = minutes & 0b1111;
    uint8_t decimal = (minutes >> 4) & 0b1111;
    set_tile(25, 6, ZERO_DIGIT + decimal);
    set_tile(26, 6, ZERO_DIGIT + unit);
    set_tile(27, 6, COLON);

    unit = seconds & 0b1111;
    decimal = (seconds >> 4) & 0b1111;
    set_tile(28, 6, ZERO_DIGIT + decimal);
    set_tile(29, 6, ZERO_DIGIT + unit);
}


inline void count_bcd_mines(const minefield* mf)
{
    negative = mf->mines < 0;
    int8_t tmp = negative ? -mf->mines : mf->mines;

    units = tmp % 10;
    decimals = tmp / 10;
}


void update_counter(minefield* mf)
{
    count_bcd_mines(mf);
    set_tile(25, 3, negative ? MINUS_SIGN : NO_SIGN);
    set_tile(26, 3, ZERO_DIGIT + decimals);
    set_tile(27, 3, ZERO_DIGIT + units);
}


void idle_update(minefield* mf)
{
    mf;
}


void platform_shutdown()
{
    // Cartridge games can't unload.
    __asm__("jp 0");
}
