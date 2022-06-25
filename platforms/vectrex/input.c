#include "common.h"
#include "minefield.h"

char joy = 0;
char buttons = 0;

#define JOY1_BTN1_MASK (1 << 0)
#define JOY1_BTN2_MASK (1 << 1)
#define JOY1_BTN3_MASK (1 << 2)
#define JOY1_BTN4_MASK (1 << 3)

uint8_t input_read(uint8_t source)
{
	unsigned char key = MINE_INPUT_IGNORED;

    if (buttons & JOY1_BTN1_MASK) key = MINE_INPUT_OPEN;
    if (buttons & JOY1_BTN2_MASK) key = MINE_INPUT_OPEN_BLOCK;
    if (buttons & JOY1_BTN3_MASK) key = MINE_INPUT_FLAG;
    if (buttons & JOY1_BTN4_MASK) key = MINE_INPUT_QUIT;
    if (joy & (1 << 0)) key = MINE_INPUT_UP;
    if (joy & (1 << 1)) key = MINE_INPUT_DOWN;
    if (joy & (1 << 2)) key = MINE_INPUT_LEFT;
    if (joy & (1 << 3)) key = MINE_INPUT_RIGHT;

	return key;
}


inline uint8_t vectrex_bios_Random(){
    // This routine generates a random 1-byte number,
    // and places it in the A register.
    register uint8_t number __asm__("a");
    __asm__ __volatile("jsr 0xf517");
    return number;
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
    return (vectrex_bios_Random() % (hi - low)) + low;
}
