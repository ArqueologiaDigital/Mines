#include "../../common/minefield.h"
#include <ncurses.h>
#include <stdlib.h>

uint8 wait_for_any_key(){
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
