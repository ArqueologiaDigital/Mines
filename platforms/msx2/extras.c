#include <stdint.h>
#include "common.h"
#include "msx2.h"
#include "minefield.h"


#define WIDTH 10
#define HEIGHT 10

static minefield _minefield;
static uint8_t _cells[WIDTH * HEIGHT];


extern void video_init();
extern void enable_vblank();


void platform_init()
{
    set_random_seed(read_clock());
    video_init();
    enable_vblank();
}


void platform_shutdown()
{
    // Cartridge games can't unload.
    __asm__("jp 0");
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


minefield* init_minefield()
{
    minefield* mf = &_minefield;
    mf->state = TITLE_SCREEN;
    mf->width = WIDTH;
    mf->height = HEIGHT;
    mf->current_cell = 0;
    mf->cells = _cells;

    return mf;
}


void free_minefield(minefield* mf)
{
    mf;
}

