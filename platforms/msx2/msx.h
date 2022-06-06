#ifndef MSX_H
#define MSX_H

/* __sdcccall function attribute on SDCC 4.1.12 and later */
#if (__SDCC_VERSION_MAJOR > 4 || \
    (__SDCC_VERSION_MAJOR == 4 && __SDCC_VERSION_MINOR > 1) || \
    (__SDCC_VERSION_MAJOR == 4 && __SDCC_VERSION_MINOR == 1 && __SDCC_VERSION_PATCH >= 12))
#define SDCCCALL __sdcccall(0)
#else
#define SDCCCALL
#endif

/* fake tile-based screen used by set_char */
#define SCREEN_WIDTH 32
#define SCREEN_HEIGHT 25

void write_vdp(uint8_t reg, uint8_t data) SDCCCALL;

void write_vram(uint8_t *dst, uint16_t len, uint8_t *src) SDCCCALL;

void fill_vram(uint8_t byte, uint16_t len, uint8_t *dst) SDCCCALL;

void set_colors(uint8_t fg, uint8_t bg, uint8_t bd) SDCCCALL;

void enable_screen();

void disable_screen();

void lmmm(uint16_t sx, uint16_t sy, uint16_t dx, uint16_t dy, uint16_t width, uint16_t height) SDCCCALL;

#endif /* MSX_H */
