/* input.c: Control panel input driver for KN5000
 *
 * Reads button state from firmware RAM. The main firmware's SC1 interrupt-
 * driven state machine continuously polls the control panel and stores
 * button bitmaps in RAM arrays. We simply read those arrays — no direct
 * SC1 serial access needed, no interference with firmware operation.
 *
 * Button state arrays (1 byte per segment, bit = button pressed):
 *   Right panel: 0x8E4A + segment  (segments 0-10)
 *   Left panel:  0x8E5A + segment  (segments 0-10)
 */

#include "common.h"
#include "minefield.h"
#include "kn5000.h"

/* Forward declarations for functions provided by extras.c */
extern int rand(void);

/* ========================================================================
 * Firmware RAM button state arrays
 *
 * WORKAROUND for LLVM TLCS-900 bug #8: 8-bit register encoding mismatches
 * corrupt byte loads, so values end up in wrong registers. All reads use
 * 32-bit aligned loads with shift/mask to extract individual bytes.
 *
 * Memory layout (little-endian):
 *   Right panel: 0x8E4A + segment (segs 0-10)
 *   Left panel:  0x8E5A + segment (segs 0-10)
 *
 * Aligned 32-bit reads:
 *   0x8E4C → [CPR_SEG2, CPR_SEG3, CPR_SEG4, CPR_SEG5]  (bits 0-7, 8-15, 16-23, 24-31)
 *   0x8E5C → [CPL_SEG2, CPL_SEG3, CPL_SEG4, CPL_SEG5]
 *   0x8E60 → [CPL_SEG6, CPL_SEG7, CPL_SEG8, CPL_SEG9]
 * ======================================================================== */

/* ========================================================================
 * Input reading
 * ======================================================================== */

/* Previous button state for edge detection (press, not hold) */
static uint32_t prev_buttons = 0;

uint8_t input_read(uint8_t source)
{
    (void)source;

    /* Read button state using 16-bit aligned loads.
     * 16-bit register encoding is also correct in LLVM TLCS-900.
     * Little-endian: low byte is the even address. */
    uint16_t cpr_word16 = *(volatile uint16_t *)0x8E4E; /* [SEG5 | SEG4] */
    uint16_t cpl_word16 = *(volatile uint16_t *)0x8E5E; /* [SEG5 | SEG4] */
    uint16_t cpl_word16_2 = *(volatile uint16_t *)0x8E60; /* [SEG7 | SEG6] */

    /* Extract segments (low byte of 16-bit word) */
    uint32_t cpr_seg4 = cpr_word16 & 0xFF;
    uint32_t cpl_seg4 = cpl_word16 & 0xFF;
    uint32_t cpl_seg7 = (cpl_word16_2 >> 8) & 0xFF;

    /* Build raw button state */
    uint32_t buttons = 0;

    if (cpr_seg4 & CPR_SEG4_UP)     buttons |= MINE_INPUT_UP;
    if (cpr_seg4 & CPR_SEG4_DOWN)   buttons |= MINE_INPUT_DOWN;
    if (cpr_seg4 & CPR_SEG4_LEFT)   buttons |= MINE_INPUT_LEFT;
    if (cpr_seg4 & CPR_SEG4_RIGHT)  buttons |= MINE_INPUT_RIGHT;
    if (cpl_seg4 & CPL_SEG4_OPEN)   buttons |= MINE_INPUT_OPEN;
    if (cpl_seg4 & CPL_SEG4_FLAG)   buttons |= MINE_INPUT_FLAG;
    if (cpl_seg7 & CPL_SEG7_QUIT)   buttons |= MINE_INPUT_QUIT;

    /* Edge detection: return only newly pressed buttons */
    uint32_t pressed = buttons & ~prev_buttons;
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
