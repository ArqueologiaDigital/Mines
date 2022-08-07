#include "msx2.h"
#include "common.h"
#include "minefield.h"


/* Time controls */
extern uint8_t ticks;       /* add one tick each time HTIMI is called */
extern uint8_t seconds;     /* makeshift BCD counter */
extern uint8_t minutes;     /* makeshift BCD counter */


extern void count_bcd_time();
