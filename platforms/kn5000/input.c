/* input.c: Control panel input driver for KN5000
 *
 * Reads button state from the KN5000 control panel via SC1 synchronous serial.
 * The SC1 hardware is initialized by the assembly wrapper (boot_hw_init).
 *
 * Protocol: send command byte + segment byte + 2 dummy bytes (0xFF)
 * to clock in header (discarded) and button bitmap.
 */

#include "common.h"
#include "minefield.h"
#include "kn5000.h"

/* Forward declarations for functions provided by extras.c */
extern int rand(void);

/* ========================================================================
 * SC1 Serial I/O
 * ======================================================================== */

/* Send one byte via SC1 and receive one byte back (synchronous mode).
 * In sync mode, writing SC1BUF simultaneously sends and receives. */
static uint8_t cpanel_send_byte(uint8_t byte)
{
    INTCLR = INTRX1_CLR;       /* Clear INTRX1 pending flag */
    SC1BUF = byte;             /* Start 8-bit synchronous transfer */

    /* Poll for RX complete with timeout */
    uint16_t timeout = 0;
    do {
        if (INTES1 & INTRX1_BIT)
            return SC1BUF;     /* Read received byte */
        timeout++;
    } while (timeout != 0);    /* Wraps to 0 after 65536 iterations */

    return 0;                  /* Timeout */
}

/* Query a control panel button segment.
 * cmd: 0x20 = left panel, 0xE0 = right panel
 * segment: segment number to query
 * Returns: button bitmap */
static uint8_t cpanel_query_segment(uint8_t cmd, uint8_t segment)
{
    cpanel_send_byte(cmd);           /* Send command */
    cpanel_send_byte(segment);       /* Send segment number */
    cpanel_send_byte(0xFF);          /* Clock in header (discard) */
    return cpanel_send_byte(0xFF);   /* Clock in button bitmap */
}

/* ========================================================================
 * Input reading
 * ======================================================================== */

/* Previous button state for edge detection (press, not hold) */
static uint8_t prev_buttons = 0;

uint8_t input_read(uint8_t source)
{
    (void)source;

    /* Query all needed control panel segments */
    uint8_t cpr_seg4 = cpanel_query_segment(CPANEL_RIGHT, CPR_SEG4);
    uint8_t cpl_seg4 = cpanel_query_segment(CPANEL_LEFT,  CPL_SEG4);
    uint8_t cpl_seg2 = cpanel_query_segment(CPANEL_LEFT,  CPL_SEG2);
    uint8_t cpl_seg7 = cpanel_query_segment(CPANEL_LEFT,  CPL_SEG7);

    /* Build raw button state */
    uint8_t buttons = 0;

    if (cpr_seg4 & CPR_SEG4_UP)     buttons |= MINE_INPUT_UP;
    if (cpr_seg4 & CPR_SEG4_DOWN)   buttons |= MINE_INPUT_DOWN;
    if (cpr_seg4 & CPR_SEG4_LEFT)   buttons |= MINE_INPUT_LEFT;
    if (cpr_seg4 & CPR_SEG4_RIGHT)  buttons |= MINE_INPUT_RIGHT;
    if (cpl_seg4 & CPL_SEG4_OPEN)   buttons |= MINE_INPUT_OPEN;
    if (cpl_seg2 & CPL_SEG2_FLAG)   buttons |= MINE_INPUT_FLAG;
    if (cpl_seg7 & CPL_SEG7_QUIT)   buttons |= MINE_INPUT_QUIT;

    /* Edge detection: return only newly pressed buttons */
    uint8_t pressed = buttons & ~prev_buttons;
    prev_buttons = buttons;

    if (pressed == 0)
        return MINE_INPUT_IGNORED;

    /* Return highest-priority input (only one action per frame) */
    if (pressed & MINE_INPUT_QUIT)   return MINE_INPUT_QUIT;
    if (pressed & MINE_INPUT_OPEN)   return MINE_INPUT_OPEN;
    if (pressed & MINE_INPUT_FLAG)   return MINE_INPUT_FLAG;
    if (pressed & MINE_INPUT_UP)     return MINE_INPUT_UP;
    if (pressed & MINE_INPUT_DOWN)   return MINE_INPUT_DOWN;
    if (pressed & MINE_INPUT_LEFT)   return MINE_INPUT_LEFT;
    if (pressed & MINE_INPUT_RIGHT)  return MINE_INPUT_RIGHT;

    return MINE_INPUT_IGNORED;
}

/* ========================================================================
 * Random number generator
 * ======================================================================== */

static inline uint16_t nearly_divisionless(uint16_t s)
{
    /* 16-bit version of Lemire's "Nearly Divisionless Random Integer
     * Generation" routine. Paper: https://arxiv.org/abs/1805.10941 */
    uint16_t x = (uint16_t)rand();
    uint32_t m = (uint32_t)x * (uint32_t)s;
    uint16_t l = (uint16_t)m;

    if (l < s) {
        uint16_t t = (uint16_t)(-(int16_t)s) % s;
        while (l < t) {
            x = (uint16_t)rand();
            m = (uint32_t)x * (uint32_t)s;
            l = (uint16_t)m;
        }
    }

    return (uint16_t)(m >> 16);
}

int random_number(int min_num, int max_num)
{
    return nearly_divisionless((uint16_t)(max_num - min_num)) + min_num;
}
