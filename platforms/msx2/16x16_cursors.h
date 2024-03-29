#include "msx2.h"

/* cursor sprite */
#define CURSOR_PATTERN_ID 0
#define CURSOR_SPRITE_ID 0
#define CURSOR_X_OFFSET 0
#define CURSOR_Y_OFFSET 0

const uint8_t cursor_pattern[32] = {
    0x00,0x70,0x40,0x40,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x40,0x40,0x70,0x00,
    0x00,0x0E,0x02,0x02,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x02,0x02,0x0E,0x00
};


/* all black */
const uint8_t cursor_colors[16] = {
    0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
    0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
};


/* mouse sprite */
#define MOUSE_PATTERN_ID 4
#define MOUSE_SPRITE_ID 1

const uint8_t mouse_pattern[32] = {
    0x01,0x01,0x01,0x01,0x01,0x03,0x04,0xFD,
    0x04,0x03,0x01,0x01,0x01,0x01,0x01,0x00,
    0x00,0x00,0x00,0x00,0x00,0x80,0x40,0x7E,
    0x40,0x80,0x00,0x00,0x00,0x00,0x00,0x00
};


/* all black */
const uint8_t mouse_colors[16] = {
    0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
    0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
};
