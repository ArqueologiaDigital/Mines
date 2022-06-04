#include "minefield.h"
#include <stdlib.h>
#include <time.h>

#include "char_codes.h"

#define SYSTEM ((uint8_t*) 0xC000)
#define DSW1 ((uint8_t*) 0xC003)
#define DSW2 ((uint8_t*) 0xC004)
#define SOUND_COMMAND ((uint8_t*) 0xC800)
#define HWCFG ((uint8_t*) 0xC804)
#define NO_SOUND 0xff

#define VIDEORAM ((uint8_t*) 0xD000) //videoram area: D000-D3FF
#define COLORRAM ((uint8_t*) 0xD400) //videoram area: D400-D7FF
#define SCROLLX ((uint8_t*) 0xD800)
#define SCROLLY ((uint8_t*) 0xD802)
#define VIDEOCFG ((uint8_t*) 0xD806)
#define SPRITERAM ((uint8_t*) 0xF000) //spriteram area: F000-FFFF
#define CLEAR_COLOR 0x80 //why?!

#define SCREEN_WIDTH 28
#define SCREEN_HEIGHT 32

#define CLOSED_CELL_COLOR 0
#define ONE_BOMB_COLOR 16
#define TWO_BOMBS_COLOR 17
#define THREE_BOMBS_COLOR 21 /* OK */
#define FOUR_BOMBS_COLOR 4 /* OK */
#define FIVE_BOMBS_COLOR 20
#define SIX_BOMBS_COLOR 6 /* OK */
#define SEVEN_BOMBS_COLOR 7
#define EIGHT_BOMBS_COLOR 8
#define FLAG_COLOR 9  /* OK */
#define SCENARIO_COLOR 15
#define UNCOVERED_BOMB_COLOR 10
#define EXPLODING_BOMB_COLOR 11
#define MINEFIELD_GRID_COLOR 12
#define TEXT_COLOR 13
#define GRID_COLOR 14

#define bool uint8_t
#define true 0xFF
#define false 0x00

extern uint8_t input_map;
int scroll_x_pos;

void set_scrollx(int pos){
	*(SCROLLX) = pos&0xFF;
	*(SCROLLX+1) = (pos>>8)&0xFF;
}

void idle_loop(){
	//set_scrollx((scroll_x_pos++)/64);
	set_scrollx(0x3000);
}

//routine for placing a character on screen
void set_char(int x, int y, char char_code, char color){
	x += 2; // there's a portion of the screen that is not visible.

	*(COLORRAM + SCREEN_HEIGHT*(x + 1) - y - 1) = color;
	*(VIDEORAM + SCREEN_HEIGHT*(x + 1) - y - 1) = char_code;
}

// Routine to print a line of text at a
// given screen coordinate
void print_line(char* str, int x, int y, char color){
	char* ptr = str;
	while (*ptr != 0){
		set_char(x, y++, *(ptr++) - 55, color);
	}
}

void clear_screen(){
	for (int i=0; i<1024; i++)
		*(COLORRAM+i) = CLEAR_COLOR;
}

void draw_scenario(){
	int x,y;

	set_char(0, 0,
	         CORNER_TOP_LEFT,  SCENARIO_COLOR);

	set_char(SCREEN_WIDTH-1, 0,
	         CORNER_TOP_RIGHT,  SCENARIO_COLOR);

	set_char(0, SCREEN_HEIGHT-1,
	         CORNER_BOTTOM_LEFT,  SCENARIO_COLOR);

	set_char(SCREEN_WIDTH-1, SCREEN_HEIGHT-1,
	         CORNER_BOTTOM_RIGHT,  SCENARIO_COLOR);

	for (y=1; y<SCREEN_HEIGHT/2; y++){
		set_char(0, y,
		         LEFT_BORDER__TOP,  SCENARIO_COLOR);

		set_char(SCREEN_WIDTH-1, y,
		         RIGHT_BORDER__TOP,  SCENARIO_COLOR);
	}

	for (y=SCREEN_HEIGHT/2; y<SCREEN_HEIGHT-1; y++){
		set_char(0, y,
		         LEFT_BORDER__BOTTOM,  SCENARIO_COLOR);

		set_char(SCREEN_WIDTH-1, y,
		         RIGHT_BORDER__BOTTOM,  SCENARIO_COLOR);
	}

	for (x=1; x<SCREEN_WIDTH/2; x++){
		set_char(x, 0,
		         TOP_BORDER__LEFT,  SCENARIO_COLOR);

		set_char(x, SCREEN_HEIGHT-1,
		         BOTTOM_BORDER__LEFT,  SCENARIO_COLOR);
	}

	for (x=SCREEN_WIDTH/2; x<SCREEN_WIDTH-1; x++){
		set_char(x, 0,
		         TOP_BORDER__RIGHT,  SCENARIO_COLOR);

		set_char(x, SCREEN_HEIGHT-1,
		         BOTTOM_BORDER__RIGHT,  SCENARIO_COLOR);
	}
}

void init_video(){
	scroll_x_pos = 0x0000;
	*VIDEOCFG = 0x30; // enables bg / enables sprites / selects sprite3bank #0
	*HWCFG = 0x80; // unflip screen and enable chars

	clear_screen();
}

// TODO: perhaps part of this should move to a platform.c file
void platform_init()
{
	*SOUND_COMMAND = NO_SOUND;
	input_map = 0xFF;

	// Init random number generator:
	srand(time(NULL));

	init_video();
	draw_scenario();
}

void draw_minefield(minefield* mf){

	uint8_t minefield_x_position = 3;
	uint8_t minefield_y_position = 5;

	for (uint8_t x = 0; x <= mf->width; x++){
		for (uint8_t y = 0; y <= mf->height; y++){
			if (y > 0 && y <= mf->height)
				set_char(minefield_x_position + x*2,
						 minefield_y_position + y*2-1,
						 MINEFIELD_VERTICAL, GRID_COLOR);

			if (x < mf->width - 1 && y < mf->height - 1)
				set_char(minefield_x_position + x*2 + 2,
						 minefield_y_position + y*2 + 2,
						 MINEFIELD_CROSS, GRID_COLOR);

			if (x > 0 && x <= mf->width)
				set_char(minefield_x_position + x*2 - 1,
						 minefield_y_position + y*2,
						 MINEFIELD_HORIZONTAL, GRID_COLOR);
		}
	}

	set_char(minefield_x_position,
			 minefield_y_position,
			 MINEFIELD_CORNER_TOP_LEFT, GRID_COLOR);

	set_char(minefield_x_position + mf->width*2,
			 minefield_y_position,
			 MINEFIELD_CORNER_TOP_RIGHT, GRID_COLOR);

	set_char(minefield_x_position,
			 minefield_y_position + mf->height*2,
			 MINEFIELD_CORNER_BOTTOM_LEFT, GRID_COLOR);

	set_char(minefield_x_position + mf->width*2,
			 minefield_y_position + mf->height*2,
			 MINEFIELD_CORNER_BOTTOM_RIGHT, GRID_COLOR);
			 
	for (uint8_t x = 0; x <= mf->width; x++){
		if (x > 0 && x < mf->width){
			set_char(minefield_x_position + x*2,
					 minefield_y_position,
					 MINEFIELD_TOP_TEE, GRID_COLOR);

			set_char(minefield_x_position + x*2,
					 minefield_y_position + mf->height*2,
					 MINEFIELD_BOTTOM_TEE, GRID_COLOR);
		}
	}

	for (uint8_t y = 0; y <= mf->height; y++){
		if (y > 0 && y < mf->height){
			set_char(minefield_x_position,
					 minefield_y_position + y*2,
					 MINEFIELD_LEFT_TEE, GRID_COLOR);

			set_char(minefield_x_position + mf->width*2,
					 minefield_y_position + y*2,
					 MINEFIELD_RIGHT_TEE, GRID_COLOR);
		}
	}
	
	for (uint8_t x = 0; x < mf->width; x++){
		for (uint8_t y = 0; y < mf->height; y++){
			if (CELL_INDEX(mf, x, y) == mf->current_cell) {
				//move(minefield_y_position + y*2 + 1,
			    //     minefield_x_position + x*2 + 1);
				//standout();
			}
			if (CELL(mf, x, y) & ISOPEN) {
				if (CELL(mf, x, y) & HASBOMB){
					set_char(minefield_x_position + x*2 + 1,
							 minefield_y_position + y*2 + 1,
							 BOMB, UNCOVERED_BOMB_COLOR);
				} else {
					uint8_t tile_number = 0;
					uint8_t tile_color = 0;
					uint8_t count = CELL(mf, x, y) & 0x0F;
					if (count > 0){
						switch(count){
							case 1:
							{
								tile_number = ONE_BOMB;
								tile_color = ONE_BOMB_COLOR;
								break;
							}
							case 2:
							{
								tile_number = TWO_BOMBS;
								tile_color = TWO_BOMBS_COLOR;
								break;
							}
							case 3:
							{
								tile_number = THREE_BOMBS;
								tile_color = THREE_BOMBS_COLOR;
								break;
							}
							case 4:
							{
								tile_number = FOUR_BOMBS;
								tile_color = FOUR_BOMBS_COLOR;
								break;
							}
							case 5:
							{
								tile_number = FIVE_BOMBS;
								tile_color = FIVE_BOMBS_COLOR;
								break;
							}
							case 6:
							{
								tile_number = SIX_BOMBS;
								tile_color = SIX_BOMBS_COLOR;
								break;
							}
							case 7:
								tile_number = SEVEN_BOMBS;
								tile_color = SEVEN_BOMBS_COLOR;
								break;
							case 8:
								tile_number = EIGHT_BOMBS;
								tile_color = EIGHT_BOMBS_COLOR;
								break;
							default:
								// never happens ;-)
								break;
						}
						set_char(minefield_x_position + x*2 + 1, 
								 minefield_y_position + y*2 + 1,
								 tile_number, tile_color);
					} else {
						set_char(minefield_x_position + x*2 + 1,
								 minefield_y_position + y*2 + 1,
								 BLANK, GRID_COLOR);
					}
				}
			} else {
				if (CELL(mf, x, y) & HASFLAG){
					set_char(minefield_x_position + x*2 + 1,
							 minefield_y_position + y*2 + 1,
							 FLAG, FLAG_COLOR);
				} else {
					set_char(minefield_x_position + x*2 + 1,
							 minefield_y_position + y*2 + 1,
							 CLOSED_CELL, CLOSED_CELL_COLOR);
				}
			}
		}
	}
}

void shutdown()
{
}
