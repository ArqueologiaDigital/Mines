//#include "msx2.h"
#include "common.h"
//#include "game.h"
//#include "minefield.h"
#include "codes.h"
#include "tiles.h"


/* Time controls */
uint8_t ticks = 0; /* add one tick each time HTIMI is called */
uint8_t seconds = 0; /* makeshift BCD counter */
uint8_t minutes = 0; /* makeshift BCD counter */


void count_bcd_time()
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


void update_timer(minefield* mf)
{
    mf;

    uint8_t unit = minutes & 0b1111;
    uint8_t decimal = (minutes >> 4) & 0b1111;
    set_tile(HOURGLASS_X_POS + 1, HOURGLASS_Y_POS, ZERO_DIGIT + decimal);
    set_tile(HOURGLASS_X_POS + 2, HOURGLASS_Y_POS, ZERO_DIGIT + unit);
    set_tile(HOURGLASS_X_POS + 3, HOURGLASS_Y_POS, COLON);

    unit = seconds & 0b1111;
    decimal = (seconds >> 4) & 0b1111;
    set_tile(HOURGLASS_X_POS + 4, HOURGLASS_Y_POS, ZERO_DIGIT + decimal);
    set_tile(HOURGLASS_X_POS + 5, HOURGLASS_Y_POS, ZERO_DIGIT + unit);
}


void reset_timer()
{
    debug_msg("reset_timer() called\n");
    ticks = 0;
    seconds = 0;
    minutes = 0;
}
