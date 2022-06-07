#ifndef MSX_H
#define MSX_H

#include <stdint.h>

/* __sdcccall function attribute on SDCC 4.1.12 and later */
#if (__SDCC_VERSION_MAJOR > 4 || \
    (__SDCC_VERSION_MAJOR == 4 && __SDCC_VERSION_MINOR > 1) || \
    (__SDCC_VERSION_MAJOR == 4 && __SDCC_VERSION_MINOR == 1 && __SDCC_VERSION_PATCH >= 12))
#define SDCCCALL0 __sdcccall(0)
#else
#define SDCCCALL0
#endif

/* fake tile-based screen used by set_char */
#define SCREEN_WIDTH 32
#define SCREEN_HEIGHT 25

/* VDP operations */
#define OP_IMP   0b0000
#define OP_AND   0b0001
#define OP_OR    0b0010
#define OP_XOR   0b0011
#define OP_NOT   0b0100
#define OP_TIMP  0b1000
#define OP_TAND  0b1001
#define OP_TOR   0b1010
#define OP_TXOR  0b1011
#define OP_TNOT  0b1100

void write_vdp(uint8_t reg, uint8_t data) SDCCCALL0;

void write_vram(uint8_t *dst, uint16_t len, uint8_t *src) SDCCCALL0;

void fill_vram(uint8_t byte, uint16_t len, uint8_t *dst) SDCCCALL0;

void set_colors(uint8_t fg, uint8_t bg, uint8_t bd) SDCCCALL0;

void enable_screen();

void disable_screen();

void lmmm(uint16_t sx, uint16_t sy, uint16_t dx, uint16_t dy, uint16_t width, uint16_t height) SDCCCALL0;

void lmmm_op(uint16_t sx, uint16_t sy, uint16_t dx, uint16_t dy, uint16_t width, uint16_t height, uint8_t op) SDCCCALL0;

void set_palette(uint8_t count, uint8_t* color_table) SDCCCALL0;

void restore_palette();

#endif /* MSX_H */
