#include <string.h>
#include "msx2.h"
#include "minefield.h"
#include "common.h"
#include "video-tiles.h"

#define true 0xff
#define false 0x00

//#define USE_DEBUG_MODE
#include "debug.h"


extern uint8_t* mines_data;
extern uint8_t* mines_palette;


/* set_tile emulates tile behaviour, but is actually a bitmap copy */
void set_tile(uint8_t dst_x, uint8_t dst_y, uint8_t tile)
{
    /* copy 8x8 block from page 1 (hidden page) to page 0 (visible page) */
    vdp(tile % 12 * 8, tile / 12 * 8 + 256, dst_x * 8, dst_y * 8, 8, 8, DIR_DEFAULT, VDP_LMMM | PO_IMP);
}


void video_init()
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

    //restore_palette();
    set_palette(15, (uint8_t*) &mines_palette);

    /* Move board and mines from RAM to second VRAM page */
    write_vram((uint8_t*) 0x8000, 0x1800, (uint8_t*) &mines_data);

    enable_screen();
}


void highlight_cell(int x, int y)
{
    //put_sprite(cursor, x, y + 1);
    /* merge cursor block with tile from page 0 (visible page) */
    vdp(CURSOR % 12 * 8, CURSOR / 12 * 8 + 256, x * 8, y * 8, 8, 8, DIR_DEFAULT, VDP_LMMM | PO_XOR);
}


void platform_init()
{
    set_random_seed(read_clock());
    video_init();
    draw_scenario();
}


void idle_loop(minefield* mf)
{
    mf;
}


void platform_shutdown()
{
    // Cartridge games can't unload.
    __asm__("jp 0");
}
