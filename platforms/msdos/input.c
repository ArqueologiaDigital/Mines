#include "common.h"
#include "minefield.h"
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>

// Uncomment this to see the keyboard scan code
// values drawn to screen using game tiles:
//#define DEBUGGING

#ifdef DEBUGGING
#include "video-tiles.h"
#endif

uint8_t input_read(uint8_t source)
{
	char c = getch();
#ifdef DEBUGGING
	set_tile(6,10, ONE_BOMB + c%10 - 1);
	set_tile(5,10, ONE_BOMB + (c/10)%10 - 1);
	set_tile(4,10, ONE_BOMB + (c/100)%10 - 1);
#endif

	if (c == 0 || c == -32) {
		c = getch();
		switch(c) {
		    case 72: return MINE_INPUT_UP;
		    case 80: return MINE_INPUT_DOWN;
		    case 77: return MINE_INPUT_RIGHT;
		    case 75: return MINE_INPUT_LEFT;
			default: break;
		}
	} else if (c == (int)'\n' || c == (int)' ') {
		return MINE_INPUT_OPEN;
	} else if (c == (int)'b' || c == (int)'B') {
		return MINE_INPUT_OPEN_BLOCK;
	} else if (c == (int)'f' || c == (int)'F') {
		return MINE_INPUT_FLAG;
	} else if (c == (int)'q' || c == (int)'Q' || c == 27) {
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
