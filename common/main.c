#include "main.h"

void main(){
	platform_init();

	minefield mf;
	uint8 width = 10;
	uint8 height = 10;
	uint8 num_bombs = 15;
	// TODO: let platforms configure specific values for
	//       minefield dimensions and number of bombs
	
	setup_minefield(&mf, width, height, num_bombs);
	draw_minefield(&mf);

	wait_for_any_key();
	shutdown();
}
