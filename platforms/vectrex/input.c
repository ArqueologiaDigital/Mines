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


int random_number(int min_num, int max_num)
{
	int result = 0, low_num = 0, hi_num = 0;

	if (min_num < max_num)
	{
		low_num = min_num;
		hi_num = max_num + 1; // include max_num in output
	} else {
		low_num = max_num + 1; // include max_num in output
		hi_num = min_num;
	}

	result = (rand() % (hi_num - low_num)) + low_num;
	return result;
}
