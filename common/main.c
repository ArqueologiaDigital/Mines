#include "main.h"

void game_over(){
	// TODO: Implement-me!
}

void main(){
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
		uint8_t input = wait_for_any_key();
		uint8_t x = mf.current_cell % mf.width;
		uint8_t y = mf.current_cell / mf.width;
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
}
