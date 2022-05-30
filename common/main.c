#include "main.h"

void main(){
	platform_init();

	minefield mf;
	uint8 width = 10;
	uint8 height = 10;
        uint8 num_cells = (width * height);
	uint8 num_bombs = random_number(15,60);
	// TODO: let platforms configure specific values for
	//       minefield dimensions and number of bombs

	setup_minefield(&mf, width, height, num_bombs);
	draw_minefield(&mf);

        while (1) {
          uint8 input = wait_for_any_key();
          if (input == MINE_INPUT_LEFT) {
            if (mf.current_cell > 0) mf.current_cell--;
          } else if (input == MINE_INPUT_RIGHT) {
            if (mf.current_cell < num_cells - 1) mf.current_cell++;
          } else if (input == MINE_INPUT_OPEN) {
            mf.cells[mf.current_cell] |= ISOPEN;
          }
          draw_minefield(&mf);
        }
	shutdown();
}
