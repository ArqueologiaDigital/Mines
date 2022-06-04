#include "minefield.h"
#include "input.h"
#include <stdlib.h>

#define P1 ((char*) 0xC001)
#define P2 ((char*) 0xC002)

char input_map;

uint8_t input_read(uint8_t source){
	char a = ~input_map ^ *P1;

	if (source == KEYBOARD){ // This looks incorrect :-P
		if ((a & (1 << 0))==0) return MINE_INPUT_RIGHT;
		if ((a & (1 << 1))==0) return MINE_INPUT_LEFT;
		if ((a & (1 << 2))==0) return MINE_INPUT_DOWN;
		if ((a & (1 << 3))==0) return MINE_INPUT_UP;
		if ((a & (1 << 4))==0) return MINE_INPUT_FLAG;
		if ((a & (1 << 5))==0) return MINE_INPUT_OPEN;
		if ((a & (1 << 6))==0) return MINE_INPUT_OPEN_BLOCK;
		if ((a & (1 << 7))==0) return MINE_INPUT_QUIT;
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
