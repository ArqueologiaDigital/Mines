#include "common.h"
#include "minefield.h"
#include <ncurses.h>
#include <stdlib.h>
#include <time.h>


uint8_t input_read(uint8_t source)
{
	int c = getch();

	if (c == KEY_LEFT) {
		return MINE_INPUT_LEFT;
	} else if (c == KEY_RIGHT) {
		return MINE_INPUT_RIGHT;
	} else if (c == KEY_UP) {
		return MINE_INPUT_UP;
	} else if (c == KEY_DOWN) {
		return MINE_INPUT_DOWN;
	} else if (c == (int)'\n' || c == (int)' ') {
		return MINE_INPUT_OPEN;
	} else if (c == (int)'b' || c == (int)'B') {
		return MINE_INPUT_OPEN_BLOCK;
	} else if (c == (int)'f' || c == (int)'F') {
		return MINE_INPUT_FLAG;
	} else if (c == (int)'q' || c == (int)'Q') {
		return MINE_INPUT_QUIT;
	}

	return MINE_INPUT_IGNORED;
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
