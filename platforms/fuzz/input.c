#include "common.h"
#include "minefield.h"
#include "game.h"
#include <stdlib.h>

static const uint8_t *fuzz_data, *fuzz_data_end;

/* main.c functions are not usually called directly, but the other way around, so we need to include it here. */
extern void main_gameplay_loop(minefield* mf);

uint8_t input_read(uint8_t source)
{
	if (fuzz_data < fuzz_data_end) {
		uint8_t ret = *fuzz_data;

		fuzz_data++;

		switch (ret) {
		case 'L':
			return MINE_INPUT_LEFT;
		case 'R':
			return MINE_INPUT_RIGHT;
		case 'U':
			return MINE_INPUT_UP;
		case 'D':
			return MINE_INPUT_DOWN;
		case '\n':
			return MINE_INPUT_OPEN;
		case 'B':
			return MINE_INPUT_OPEN_BLOCK;
		case 'F':
			return MINE_INPUT_FLAG;
		case 'Q':
			return MINE_INPUT_QUIT;
		}
	}

	return MINE_INPUT_IGNORED;
}


int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
	fuzz_data = data;
	fuzz_data_end = data + size;

	minefield mf;

	setup_minefield(&mf, 10, 10, 5);
	mf.state = PLAYING_GAME;

	while (mf.state != QUIT)
		gameplay_loop(&mf);

	free(mf.cells);
	return 0;
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
