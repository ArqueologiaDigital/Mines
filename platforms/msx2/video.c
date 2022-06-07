#include <stdint.h>
#include <string.h>
#include "msx.h"
#include "minefield.h"
#include "input.h"
#include "char_codes.h"

extern uint8_t* mines_data;


/* set_char emulates tile behaviour, but is actually a bitmap copy */
void set_char(uint8_t dst_x, uint8_t dst_y, uint8_t tile)
{
    /* copy 8x8 block from page 1 (hidden page) to page 0 (visible page) */
    lmmm(tile % 12 * 8, tile / 12 * 8 + 256, dst_x * 8, dst_y * 8, 8, 8);
}


static const uint8_t bg[] = { 1, 0x1, 0x1, 0x4 };


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
    set_palette(1, (uint8_t*) bg);

    /* Move board and mines from RAM to second VRAM page */
    write_vram((uint8_t*) 0x8000, 0x1800, (uint8_t*) &mines_data);

    enable_screen();
}


void draw_scenario()
{
    set_char(0, 0, 31);
    set_char(31, 0, 34);
    set_char(0, 25, 67);
    set_char(31, 25, 70);

    for (uint8_t i = 1; i < SCREEN_WIDTH / 2; ++i) {
        set_char(i, 0, 32);
        set_char(i, 25, 68);
    }

    for (uint8_t i = SCREEN_WIDTH / 2; i < SCREEN_WIDTH - 1; ++i) {
        set_char(i, 0, 33);
        set_char(i, 25, 69);
    }

    for (uint8_t j = 1; j < SCREEN_HEIGHT / 2; ++j) {
        set_char(0, j, 43);
        set_char(31, j, 46);
    }

    for (uint8_t j = SCREEN_HEIGHT / 2; j < SCREEN_HEIGHT; ++j) {
        set_char(0, j, 55);
        set_char(31, j, 58);
    }
}


void highlight_cell(int x, int y)
{
    //put_sprite(cursor, x, y + 1);
    /* merge cursor block with tile from page 0 (visible page) */
    lmmm_op(CURSOR % 12 * 8, CURSOR / 12 * 8 + 256, x * 8, y * 8, 8, 8, OP_XOR);
}


#define minefield_x_position 6
#define minefield_y_position 3

void draw_minefield(minefield* mf)
{
     /* Draw minefield frame */
     for (uint8_t x = 0; x <= mf->width; x++) {
         for (uint8_t y = 0; y <= mf->height; y++) {
             if (y > 0 && y <= mf->height)
                 set_char(minefield_x_position + x * 2,
                          minefield_y_position + y * 2 - 1,
                          MINEFIELD_VERTICAL);

         if (x < mf->width - 1 && y < mf->height - 1)
                 set_char(minefield_x_position + x * 2 + 2,
                          minefield_y_position + y * 2 + 2,
                          MINEFIELD_CROSS);

         if (x > 0 && x <= mf->width)
                 set_char(minefield_x_position + x*2 - 1,
                          minefield_y_position + y*2,
                          MINEFIELD_HORIZONTAL);
         }
     }

     set_char(minefield_x_position,
              minefield_y_position,
              MINEFIELD_CORNER_TOP_LEFT);

     set_char(minefield_x_position + mf->width * 2,
              minefield_y_position,
              MINEFIELD_CORNER_TOP_RIGHT);

     set_char(minefield_x_position,
              minefield_y_position + mf->height * 2,
              MINEFIELD_CORNER_BOTTOM_LEFT);

     set_char(minefield_x_position + mf->width * 2,
              minefield_y_position + mf->height * 2,
              MINEFIELD_CORNER_BOTTOM_RIGHT);

     for (uint8_t x = 0; x <= mf->width; x++) {
         if (x > 0 && x < mf->width) {
             set_char(minefield_x_position + x * 2,
                      minefield_y_position,
                      MINEFIELD_TOP_TEE);

             set_char(minefield_x_position + x * 2,
                      minefield_y_position + mf->height * 2,
                      MINEFIELD_BOTTOM_TEE);
        }
    }

    for (uint8_t y = 0; y <= mf->height; y++) {
        if (y > 0 && y < mf->height) {
            set_char(minefield_x_position,
                     minefield_y_position + y * 2,
                     MINEFIELD_LEFT_TEE);

            set_char(minefield_x_position + mf->width * 2,
                     minefield_y_position + y * 2,
                     MINEFIELD_RIGHT_TEE);
        }
    }

    /* Draw minefield contents */
    for (uint8_t x = 0; x < mf->width; x++) {
        for (uint8_t y = 0; y < mf->height; y++) {
            if (CELL(mf, x, y) & ISOPEN) {
                if (CELL(mf, x, y) & HASBOMB) {
                    set_char(minefield_x_position + x * 2 + 1,
                             minefield_y_position + y * 2 + 1,
                             BOMB);
                } else {
                    uint8_t tile_number = 0;
                    uint8_t tile_color = 0;
                    uint8_t count = CELL(mf, x, y) & 0x0F;
                    if (count > 0) {
                        set_char(minefield_x_position + x * 2 + 1,
                                 minefield_y_position + y * 2 + 1,
                                 count - 1);
                    } else {
                        set_char(minefield_x_position + x * 2 + 1,
                                 minefield_y_position + y * 2 + 1,
                                 BLANK);
                    }
                }
            } else {
                if (CELL(mf, x, y) & HASFLAG) {
                    set_char(minefield_x_position + x * 2 + 1,
                             minefield_y_position + y * 2 + 1,
                             FLAG);
                } else if (CELL(mf, x, y) & HASQUESTIONMARK) {
                    set_char(minefield_x_position + x * 2 + 1,
                             minefield_y_position + y * 2 + 1,
                             QUESTION_MARK);
                } else {
                    set_char(minefield_x_position + x * 2 + 1,
                             minefield_y_position + y * 2 + 1,
                             CLOSED_CELL);
                }
            }
        }
    }

    uint8_t x = CELL_X(mf);
    uint8_t y = CELL_Y(mf);
    highlight_cell(minefield_x_position + x * 2 + 1,
                   minefield_y_position + y * 2 + 1);
}


void platform_init()
{

    video_init();

    draw_scenario();
}


void idle_loop()
{
}


void shutdown()
{
    // Cartridge games can't unload.
}
