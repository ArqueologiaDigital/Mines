#include <stdint.h>
#include <string.h>
#include "msx.h"
#include "minefield.h"
#include "input.h"

extern uint8_t* mines_data;


/* set_char emulates tile behaviour, but is actually a bitmap copy */
void set_char(uint8_t dst_x, uint8_t dst_y, uint8_t tile, uint8_t color)
{
    color;
    /* copy 8x8 block from page 1 (hidden page) to page 0 (visible page) */
    lmmm(tile % 12 * 8, tile / 12 * 8 + 256, dst_x * 8, dst_y * 8, 8, 8);
}


void set_video()
{
    disable_screen();

    set_colors(15, 1, 1);

    /* Setting SCREEN5, 16x16 sprites, display disabled, vblank disabled */
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

    /* Move board and mines from RAM to second VRAM page */
    write_vram((uint8_t*) 0x8000, 0x1800, (uint8_t*) &mines_data);

    enable_screen();
}


void draw_scenario()
{
    set_char(0, 0, 31, 0);
    set_char(31, 0, 34, 0);
    set_char(0, 25, 67, 0);
    set_char(31, 25, 70, 0);

    for (uint8_t i = 1; i < SCREEN_WIDTH / 2; ++i) {
        set_char(i, 0, 32, 0);
        set_char(i, 25, 68, 0);
    }

    for (uint8_t i = SCREEN_WIDTH / 2; i < SCREEN_WIDTH - 1; ++i) {
        set_char(i, 0, 33, 0);
        set_char(i, 25, 69, 0);
    }

    for (uint8_t j = 1; j < SCREEN_HEIGHT / 2; ++j) {
        set_char(0, j, 43, 0);
        set_char(31, j, 46, 0);
    }

    for (uint8_t j = SCREEN_HEIGHT / 2; j < SCREEN_HEIGHT; ++j) {
        set_char(0, j, 55, 0);
        set_char(31, j, 58, 0);
    }
}


void platform_init()
{

    set_video();

    draw_scenario();

    while (1)
        input_read(KEYBOARD);
}


void draw_minefield(minefield* mf)
{
    /* TODO: implement me! */
    mf;
}


void idle_loop()
{
}


void shutdown()
{
    // Cartridge games can't unload.
}
