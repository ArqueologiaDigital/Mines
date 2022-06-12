#include "common.h"
#include "main.h"


uint8_t count_surrounding_flags(minefield* mf, uint8_t x, uint8_t y){
	uint8_t count = 0;
	if (CELL(mf, x-1, y-1) & HASFLAG) count++;
	if (CELL(mf, x-1, y) & HASFLAG) count++;
	if (CELL(mf, x-1, y+1) & HASFLAG) count++;

	if (CELL(mf, x, y-1) & HASFLAG) count++;
	if (CELL(mf, x, y+1) & HASFLAG) count++;

	if (CELL(mf, x+1, y-1) & HASFLAG) count++;
	if (CELL(mf, x+1, y) & HASFLAG) count++;
	if (CELL(mf, x+1, y+1) & HASFLAG) count++;

	return count;
}


void main_gameplay_loop(minefield* mf) {
	uint8_t input;
	draw_minefield_contents(mf);

	do {
		idle_loop(mf); // useful for doing other things
	                   // such as running animations
		input = input_read(KEYBOARD);
	} while (input == MINE_INPUT_IGNORED);
	debug("\nkey code = ", input);

	uint8_t x = mf->current_cell % mf->width;
	uint8_t y = mf->current_cell / mf->width;
	uint8_t has_bomb = (mf->cells[mf->current_cell] & HASBOMB);
	uint8_t num_bombs_around = (mf->cells[mf->current_cell] & 0x0F);
	uint8_t num_cells = (mf->width * mf->height);

	switch (input) {
		case MINE_INPUT_LEFT:
			if (mf->current_cell % mf->width > 0)
				mf->current_cell--;
			break;

		case MINE_INPUT_RIGHT:
			if (mf->current_cell % mf->width < mf->width - 1)
				mf->current_cell++;
			break;

		case MINE_INPUT_UP:
			if (mf->current_cell > mf->width - 1)
				mf->current_cell -= mf->width;
			break;

		case MINE_INPUT_DOWN:
			if (mf->current_cell < num_cells - mf->width)
				mf->current_cell += mf->width;
			break;

		case MINE_INPUT_OPEN:
			open_cell(mf, x, y);
			break;

		case MINE_INPUT_OPEN_BLOCK:
			if (!has_bomb && (count_surrounding_flags(mf, x, y) == num_bombs_around)) {
				open_cell(mf, x-1, y-1);
				open_cell(mf, x-1, y);
				open_cell(mf, x-1, y+1);
				open_cell(mf, x, y-1);
				open_cell(mf, x, y);
				open_cell(mf, x, y+1);
				open_cell(mf, x+1, y-1);
				open_cell(mf, x+1, y);
				open_cell(mf, x+1, y+1);
			}
			break;

		case MINE_INPUT_FLAG:
			if (!(mf->cells[mf->current_cell] & ISOPEN)) {
				if (mf->cells[mf->current_cell] & HASFLAG) {
					mf->cells[mf->current_cell] &= ~HASFLAG;
					mf->cells[mf->current_cell] |= HASQUESTIONMARK;
				} else if (mf->cells[mf->current_cell] & HASQUESTIONMARK) {
					mf->cells[mf->current_cell] &= ~HASQUESTIONMARK;
				} else {
					mf->cells[mf->current_cell] |= HASFLAG;
				}
			}
			break;

		case MINE_INPUT_QUIT:
			mf->state = QUIT;

		default:
			break;
	}
}

#if !defined(FUZZING_BUILD_MODE_UNSAFE_FOR_PRODUCTION)
int main() {
	platform_init();

	minefield mf;
	uint8_t width = 10;
	uint8_t height = 10;

	uint8_t num_bombs = random_number(10,30);
	debug("num_bombs = ", num_bombs);

	// TODO: let platforms configure specific values for
	//       minefield dimensions and number of bombs

	setup_minefield(&mf, width, height, num_bombs);
	draw_minefield(&mf);

	mf.state = PLAYING_GAME;
	while (mf.state != QUIT)
	{
		main_gameplay_loop(&mf);
	}

	free_minefield(&mf);
	platform_shutdown();

	return 0;
}
#endif
