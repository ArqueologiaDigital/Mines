#include "main.h"
#include "input.h"

void game_over() {
	// TODO: Implement-me!
}

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

int main() {
	platform_init();

	minefield mf;
	uint8_t width = 10;
	uint8_t height = 10;
	uint8_t num_cells = (width * height);
	uint8_t num_bombs = random_number(15,60);
	// TODO: let platforms configure specific values for
	//       minefield dimensions and number of bombs

	setup_minefield(&mf, width, height, num_bombs);
	draw_minefield(&mf);

	while (1) {
		uint8_t input;

		do {
			input = input_read(KEYBOARD);
		} while (input == MINE_INPUT_IGNORED);

		uint8_t x = mf.current_cell % mf.width;
		uint8_t y = mf.current_cell / mf.width;
		uint8_t has_bomb = (mf.cells[mf.current_cell] & HASBOMB);
		uint8_t num_bombs_around = (mf.cells[mf.current_cell] & 0x0F);
		switch(input){
			case MINE_INPUT_LEFT:
				if (mf.current_cell > 0)
					mf.current_cell--;
				break;

			case MINE_INPUT_RIGHT:
				if (mf.current_cell < num_cells - 1)
					mf.current_cell++;
				break;

			case MINE_INPUT_UP:
				if (mf.current_cell > mf.width - 1)
					mf.current_cell -= mf.width;
				break;

			case MINE_INPUT_DOWN:
				if (mf.current_cell < num_cells - mf.width)
					mf.current_cell += mf.width;
				break;

			case MINE_INPUT_OPEN:
				open_cell(&mf, x, y);
				break;

			case MINE_INPUT_OPEN_BLOCK:
			    if (!has_bomb && (count_surrounding_flags(&mf, x, y) == num_bombs_around)){
					open_cell(&mf, x-1, y-1);
					open_cell(&mf, x-1, y);
					open_cell(&mf, x-1, y+1);
					open_cell(&mf, x, y-1);
					open_cell(&mf, x, y);
					open_cell(&mf, x, y+1);
					open_cell(&mf, x+1, y-1);
					open_cell(&mf, x+1, y);
					open_cell(&mf, x+1, y+1);
				}
				break;

			case MINE_INPUT_FLAG:
				if (mf.cells[mf.current_cell] & HASFLAG){
					mf.cells[mf.current_cell] &= ~HASFLAG;
				} else {
					mf.cells[mf.current_cell] |= HASFLAG;
				}
				break;

			default:
				break;
		}
		draw_minefield(&mf);
	}
	//shutdown();
	return 0;
}
