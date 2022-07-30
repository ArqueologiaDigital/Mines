#ifndef MSX2_H
#define MSX2_H

#include <stdint.h>

/* __sdcccall function attribute on SDCC 4.1.12 and later */
#if (__SDCC_VERSION_MAJOR > 4 || \
    (__SDCC_VERSION_MAJOR == 4 && __SDCC_VERSION_MINOR > 1) || \
    (__SDCC_VERSION_MAJOR == 4 && __SDCC_VERSION_MINOR == 1 && __SDCC_VERSION_PATCH >= 12))
#define SDCCCALL0 __sdcccall(0)
#else
#define SDCCCALL0
#endif

/* VDP copy direction (high byte):
 * bit #5,#4: b00 = VRAM->VRAM
 * bit #3: b0 = top->bottom
 * bit #2: b0 = left->right */
#define DIR_DEFAULT         (0b0000 << 8)

/* pixel operation */
#define PO_IMP              0b0000
#define PO_AND              0b0001
#define PO_OR               0b0010
#define PO_XOR              0b0011
#define PO_NOT              0b0100
#define PO_TIMP             0b1000
#define PO_TAND             0b1001
#define PO_TOR              0b1010
#define PO_TXOR             0b1011
#define PO_TNOT             0b1100

/* VDP operation (combine it with pixel operation above) */
#define VDP_LMMM            0b10010000

/* vblank hook, 60 times per second */
#define HTIMI               0xfd9f


void write_vdp(uint8_t reg, uint8_t data) SDCCCALL0;

void write_vram(uint16_t dst, uint16_t len, uint8_t *src) SDCCCALL0;

void fill_vram(uint8_t byte, uint16_t len, uint8_t *dst) SDCCCALL0;

void set_colors(uint8_t fg, uint8_t bg, uint8_t bd) SDCCCALL0;

void enable_screen();

void disable_screen();

void vdp(uint16_t sx, uint16_t sy, uint16_t dx, uint16_t dy, uint16_t width, uint16_t height, uint16_t direction, uint8_t vdpop) SDCCCALL0;

void set_palette(uint8_t count, uint8_t* color_table) SDCCCALL0;

void restore_palette();

extern uint16_t seed;

void set_random_seed(uint16_t value) SDCCCALL0;

uint16_t xorshift() SDCCCALL0;

uint32_t read_clock() SDCCCALL0;

void set_sprite_pattern(uint8_t* src, uint8_t pattern) SDCCCALL0;

void put_sprite_colors(uint8_t* src, uint8_t index) SDCCCALL0;

void put_cursor(uint8_t x, uint8_t y);

void hide_cursor();

struct sprite_attr {
    uint8_t y;
    uint8_t x;
    uint8_t pattern_no;
    uint8_t unused;
};

/* low level sprite placement function */
void put_sprite_attr(struct sprite_attr *src, uint8_t index) SDCCCALL0;

#define IGNORE_MOUSE_THRESHOLD 10

/* search for plugged mouse device */
int8_t search_mouse();

/* structure for storing mouse data */
struct mouse {
    int8_t dx;
    int8_t dy;
    uint8_t l_button; /* 1 = OFF, 2 = ON */
    uint8_t r_button; /* 1 = OFF, 2 = ON */
};

void read_mouse(struct mouse* mouse, uint8_t source) SDCCCALL0;

void put_mouse(uint8_t x, uint8_t y);

void hide_mouse();

#endif /* MSX2_H */
