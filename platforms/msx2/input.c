#include <stdbool.h>
#include "msx2.h"
#include "ports.h"
#include "common.h"


uint8_t source_mouse = 0xff;


static uint8_t keyboard_read_row(uint8_t row) __z88dk_fastcall __naked
{
    row; assert(row <= 10);
    __asm
        in      a, (P_PPI_C)
        and     #0xf0           /* keep PPI data constant... */
        or      l               /* ...but change row */
        out     (P_PPI_C), a
        in      a, (P_PPI_B)    /* read row data */
        ld      l, a
        ret
    __endasm;
}


static uint8_t keyboard_read()
{
    uint8_t scan;
    /* previous row statuses */
    static uint8_t row4 = 0xff;
    static uint8_t row8 = 0xff;

    /* search for 'M' key */
    if ((scan = keyboard_read_row(4)) != row4) {
        row4 = scan;
        if (!(scan & (1 << 2))) return MINE_INPUT_FLAG;
        if (!(scan & (1 << 3))) return MINE_INPUT_OPEN_BLOCK;
        if (!(scan & (1 << 6))) return MINE_INPUT_QUIT;
    }
    row4 = scan;

    /* search for space and arrow keys */
    if ((scan = keyboard_read_row(8)) != row8) {
        row8 = scan;
        if (!(scan & 1)) return MINE_INPUT_OPEN;
        if (!(scan & (1 << 4))) return MINE_INPUT_LEFT;
        if (!(scan & (1 << 5))) return MINE_INPUT_UP;
        if (!(scan & (1 << 6))) return MINE_INPUT_DOWN;
        if (!(scan & (1 << 7))) return MINE_INPUT_RIGHT;
    }
    row8 = scan;

    return MINE_INPUT_IGNORED;
}


static joydata joy;


static uint8_t joystick_read(uint8_t source)
{
    uint8_t input = read_raw_joyport(source);

    if (!(input & (1 << 0))) {
        return MINE_INPUT_UP;
    }
    if (!(input & (1 << 1))) {
        return MINE_INPUT_DOWN;
    }
    if (!(input & (1 << 2))) {
        return MINE_INPUT_LEFT;
    }
    if (!(input & (1 << 3))) {
        return MINE_INPUT_RIGHT;
    }
    if (!(input & (1 << 4))) {
        debug_msg("button1\n");
        return MINE_INPUT_OPEN;
    }
    if (!(input & (1 << 5))) {
        debug_msg("button2\n");
        return MINE_INPUT_FLAG;
    }
    return MINE_INPUT_IGNORED;
}


uint8_t input_read(uint8_t source)
{
    static uint8_t wait_joy[3] = {0};

    if (source == KEYBOARD)
        return keyboard_read();
    // distinguish mouse input from joystick input
    else if (source < JOY_3 && source_mouse == 0xff) {
        uint8_t* tick = &wait_joy[source];
        if ((*tick)++ >= 30) {
            *tick = 0;
            return joystick_read(source);
        }
    }
    return MINE_INPUT_IGNORED;
}


static const uint8_t MATRIX_KEY_2_COLUMN[8][12] = {
    "08'CKS\000\000 \0005",
    "19`DLT\000\000\000\0006",
    "2-,EMU\000\x1c\000\0007",
    "3=.FNV\000\x08\00008",
    "4\\/GOW\000\000\00019",
    "5[$HPX\000\000\0002-",
    "6]AIQY\000\000\0003,",
    "7;BJRZ\000\n\0004."
};


uint8_t get_raw_ch()
{
    uint8_t key;

    do {
        key = input_read(KEYBOARD);
    } while (key == MINE_INPUT_IGNORED);

    uint8_t row = key & 0x0f;
    return MATRIX_KEY_2_COLUMN[key >> 4][row];
}


int random_number(int min, int max)
{
    int low, hi;

    if (min < max)
    {
        low = min;
        hi = max;
    } else {
        low = max;
        hi = min;
    }

    ++hi;
    return (xorshift() % (hi - low)) + low;
}

