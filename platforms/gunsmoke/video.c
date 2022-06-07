#include "minefield.h"
#include <stdlib.h>
#include <time.h>

#include "char_codes.h"

#define SOUND_COMMAND ((uint8_t*) 0xC800)
#define HWCFG ((uint8_t*) 0xC804)
#define NO_SOUND 0xff

#define VIDEORAM ((uint8_t*) 0xD000) //videoram area: D000-D3FF
#define COLORRAM ((uint8_t*) 0xD400) //videoram area: D400-D7FF
#define SCROLLY ((uint8_t*) 0xD800)
#define SCROLLX ((uint8_t*) 0xD802)
#define VIDEOCFG ((uint8_t*) 0xD806)
#define SPRITERAM ((uint8_t*) 0xF000) //spriteram area: F000-FFFF
#define CLEAR_COLOR 0x80 //why?!

#define SCREEN_WIDTH 28
#define SCREEN_HEIGHT 32

#define CLOSED_CELL_COLOR 0
#define FLAG_COLOR 9
#define QUESTION_MARK_COLOR 9
#define SCENARIO_COLOR 15
#define UNCOVERED_BOMB_COLOR 10
#define EXPLODING_BOMB_COLOR 11
#define MINEFIELD_GRID_COLOR 12
#define TEXT_COLOR 13
#define GRID_COLOR 14
#define HIGHLIGHT_CELL_COLOR 4

#define bool uint8_t
#define true 0xFF
#define false 0x00

uint8_t bomb_colors[] = {
	16, // ONE_BOMB_COLOR
	17, // TWO_BOMBS_COLOR
	21, // THREE_BOMBS_COLOR
	 4, // FOUR_BOMBS_COLOR
	20, // FIVE_BOMBS_COLOR
	 6, // SIX_BOMBS_COLOR
	 7, // SEVEN_BOMBS_COLOR
	 8  // EIGHT_BOMBS_COLOR
};

uint16_t sprite_x = 0;
uint16_t sprite_y = 0;
uint16_t target_x = 0;
uint16_t target_y = 0;
uint8_t count_loops = 0;

void set_sprite(uint8_t sprite_number, uint8_t code, uint8_t bank,
                uint8_t color, bool flipx,
                uint16_t x, uint16_t y){
	uint8_t* sprite = (SPRITERAM + 0x1000 - 32*sprite_number);
	uint8_t attr = color & 0x0f;
	attr |= ((bank & 0x03) << 6);
	attr |= ((flipx & 0x01) << 4);
	if (y <= 127){
		attr |= (1 << 5);
		y += 128;
	}
	sprite[0] = code;
	sprite[1] = attr;
	sprite[2] = (x & 0xFF);
	sprite[3] = (y & 0xFF);
}

void set_scroll_y(int pos){
	*(SCROLLY) = pos&0xFF;
	*(SCROLLY+1) = (pos>>8)&0xFF;
}

void update_sprite_position(){
//	if (sprite_x < target_x) sprite_x++;
//	else if (sprite_x > target_x) sprite_x--;

	if (count_loops % 5 > 0) return;

	if (sprite_y < target_y) sprite_y++;
	else if (sprite_y > target_y) sprite_y--;

#if 0
	set_sprite(/*sprite_number*/ (0x1000 - 0x200)/32,
               /* code */ 0x12,
               /* bank */ 0,
               /* color */ 0,
               /* flipy */ 0,
               /* x */ sprite_x,
	           /* y */ sprite_y);
#endif

   set_scroll_y(0x3400 - sprite_y);
}

void blink_cursor(minefield* mf); //prototype

void idle_loop(minefield* mf){
	update_sprite_position();
	blink_cursor(mf);
	count_loops++;
}

//routine for placing a character on screen
void set_char(int x, int y, char char_code, char color){
	x += 2; // there's a portion of the screen that is not visible.

	*(COLORRAM + SCREEN_HEIGHT*(x + 1) - y - 1) = color;
	*(VIDEORAM + SCREEN_HEIGHT*(x + 1) - y - 1) = char_code;
}

void set_color(int x, int y, char color){
	x += 2; // there's a portion of the screen that is not visible.

	*(COLORRAM + SCREEN_HEIGHT*(x + 1) - y - 1) = color;
}

void highlight_cell(int x, int y){
	char color = HIGHLIGHT_CELL_COLOR;
	set_color(x-1, y-1, color);
	set_color( x,  y-1, color);
	set_color(x+1, y-1, color);

	set_color(x-1,  y, color);
	set_color(x+1,  y, color);

	set_color(x-1, y+1, color);
	set_color( x,  y+1, color);
	set_color(x+1, y+1, color);
}

uint8_t get_char(int x, int y){
	x += 2; // there's a portion of the screen that is not visible.

	return *(VIDEORAM + SCREEN_HEIGHT*(x + 1) - y - 1);
}

uint8_t get_color(int x, int y){
	x += 2; // there's a portion of the screen that is not visible.

	return *(COLORRAM + SCREEN_HEIGHT*(x + 1) - y - 1);
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
	for (int i=0; i<1024; i++){
		*(VIDEORAM+i) = BLANK;
		*(COLORRAM+i) = CLEAR_COLOR;
	}
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

void enable_chars(){
	// a.k.a. "Foreground Tiles"
	*HWCFG |= (1 << 7);
}

void init_video(){
	*VIDEOCFG = 0x30; // enables bg / enables sprites / selects sprite3bank #0
	*HWCFG = 0x00; // screen is not flipped and chars are initially disabled

	clear_screen();
}

// TODO: perhaps part of this should move to a platform.c file
void platform_init()
{
	*SOUND_COMMAND = NO_SOUND;

	// Init random number generator:
	srand(time(NULL));

	init_video();
	draw_scenario();
}

#define minefield_x_position 3
#define minefield_y_position 5
uint8_t cursor_tile = BLANK;
uint8_t cursor_color = GRID_COLOR;

void blink_cursor(minefield* mf){
	uint8_t x = mf->current_cell % mf->width;
	uint8_t y = mf->current_cell / mf->width;
	set_char(minefield_x_position + x*2 + 1,
			 minefield_y_position + y*2 + 1,
			 ((count_loops / 32) % 2) ? BLANK : cursor_tile,
			 ((count_loops / 32) % 2) ? SCENARIO_COLOR : cursor_color);
}

void draw_minefield(minefield* mf){

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
			if (CELL(mf, x, y) & ISOPEN) {
				if (CELL(mf, x, y) & HASBOMB){
					set_char(minefield_x_position + x*2 + 1,
							 minefield_y_position + y*2 + 1,
							 BOMB, UNCOVERED_BOMB_COLOR);
				} else {
					uint8_t tile_number = 0;
					uint8_t tile_color = 0;
					uint8_t count = CELL(mf, x, y) & 0x0F;
					if (count > 0 && count < 9) {
						tile_number = ONE_BOMB + count - 1;
						tile_color = bomb_colors[count - 1];
						set_char(minefield_x_position + x * 2 + 1, 
						         minefield_y_position + y * 2 + 1,
						         tile_number, tile_color);
					} else {
						set_char(minefield_x_position + x * 2 + 1,
						         minefield_y_position + y * 2 + 1,
						         BLANK, GRID_COLOR);
					}
				}
			} else {
				if (CELL(mf, x, y) & HASFLAG){
					set_char(minefield_x_position + x*2 + 1,
							 minefield_y_position + y*2 + 1,
							 FLAG, FLAG_COLOR);
				} else if (CELL(mf, x, y) & HASQUESTIONMARK){
					set_char(minefield_x_position + x*2 + 1,
							 minefield_y_position + y*2 + 1,
							 QUESTION_MARK, QUESTION_MARK_COLOR);
				} else {
					set_char(minefield_x_position + x*2 + 1,
							 minefield_y_position + y*2 + 1,
							 CLOSED_CELL, CLOSED_CELL_COLOR);
				}
			}
		}
	}

	uint8_t x = CURRENT_CELL_X(mf);
	uint8_t y = CURRENT_CELL_Y(mf);
	target_x = 8 * (minefield_x_position + x * 2 + 1);
	target_y = 8 * (minefield_y_position + y * 2 + 1);
	cursor_tile = get_char(minefield_x_position + x * 2 + 1,
	                       minefield_y_position + y * 2 + 1);
	cursor_color = get_color(minefield_x_position + x * 2 + 1,
	                         minefield_y_position + y * 2 + 1);
	highlight_cell(minefield_x_position + x * 2 + 1,
	               minefield_y_position + y * 2 + 1);
	enable_chars();
}

void shutdown()
{
}
