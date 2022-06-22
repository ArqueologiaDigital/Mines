#include "common.h"
#include "minefield.h"
#include "video-tiles.h"
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#define SOUND_COMMAND ((uint8_t*) 0xC800)
#define HWCFG ((uint8_t*) 0xC804)
#define NO_SOUND 0xff

#define VIDEORAM ((uint8_t*) 0xD000) //videoram area: D000-D3FF
#define COLORRAM ((uint8_t*) 0xD400) //videoram area: D400-D7FF
#define SCROLLY ((uint8_t*) 0xD800)
#define SCROLLX ((uint8_t*) 0xD802)
#define VIDEOCFG ((uint8_t*) 0xD806)
#define SPRITERAM ((uint8_t*) 0xF000) //spriteram area: F000-FFFF
#define CLEAR_COLOR 0x80
#define BLINK_COLOR 22


uint8_t get_tile_color(uint8_t tile){
	switch(tile){
		case ONE_BOMB: return 16;
		case TWO_BOMBS: return 17;
		case THREE_BOMBS: return 21;
		case FOUR_BOMBS: return 4;
		case FIVE_BOMBS: return 20;
		case SIX_BOMBS: return 6;
		case SEVEN_BOMBS: return 7;
		case EIGHT_BOMBS: return 8;
		case BLANK: return 31;
		case CURSOR: return 12;
		case BOMB: return 10;
		case FLAG: return 9;
		case QUESTION_MARK: return 9;
		case EXPLOSION: return 0;
		case GROUND: return 128;

		case MINEFIELD_CORNER_TOP_LEFT:
		case MINEFIELD_TOP_TEE:
		case MINEFIELD_HORIZONTAL_TOP:
		case MINEFIELD_CORNER_TOP_RIGHT:
			return 14;

		case CORNER_TOP_LEFT:
		case TOP_BORDER__LEFT:
		case TOP_BORDER__RIGHT:
		case CORNER_TOP_RIGHT:
		case LEFT_BORDER__TOP:
		case RIGHT_BORDER__TOP:
		case LEFT_BORDER__BOTTOM:
		case RIGHT_BORDER__BOTTOM:
		case CORNER_BOTTOM_LEFT:
		case BOTTOM_BORDER__LEFT:
		case BOTTOM_BORDER__RIGHT:
		case CORNER_BOTTOM_RIGHT:
			return 12;

		case MINEFIELD_LEFT_TEE:
		case MINEFIELD_CROSS:
		case MINEFIELD_HORIZONTAL_MIDDLE:
		case MINEFIELD_VERTICAL_MIDDLE:
		case MINEFIELD_RIGHT_TEE:
		case MINEFIELD_VERTICAL_LEFT:
			return 14;

		case CLOSED_CELL: return 0;

		case MINEFIELD_VERTICAL_RIGHT:
		case MINEFIELD_CORNER_BOTTOM_LEFT:
		case MINEFIELD_BOTTOM_TEE:
		case MINEFIELD_HORIZONTAL_BOTTOM:
		case MINEFIELD_CORNER_BOTTOM_RIGHT:
			return 14;

		default: return 0;
	}
}

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

void set_scroll_y(int pos)
{
	*(SCROLLY) = pos&0xFF;
	*(SCROLLY+1) = (pos>>8)&0xFF;
}

void update_sprite_position(minefield* mf)
{
	uint8_t x = CURRENT_CELL_X(mf);
	uint8_t y = CURRENT_CELL_Y(mf);
	target_x = 8 * (MINEFIELD_X_OFFSET + x * 2 + 1);
	target_y = 8 * (MINEFIELD_Y_OFFSET + y * 2 + 1);

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

void idle_update(minefield* mf)
{
	update_sprite_position(mf);
	blink_cursor(mf);
	count_loops++;
}

//routine for placing a character on screen
void set_tile(uint8_t x, uint8_t y, uint8_t tile)
{
	x += 2; // there's a portion of the screen that is not visible.

	*(COLORRAM + SCREEN_HEIGHT*(x + 1) - y - 1) = get_tile_color(tile);
	*(VIDEORAM + SCREEN_HEIGHT*(x + 1) - y - 1) = tile;
}

uint8_t get_tile(uint8_t x, uint8_t y)
{
	x += 2; // there's a portion of the screen that is not visible.

	return *(VIDEORAM + SCREEN_HEIGHT*(x + 1) - y - 1);
}

void set_color(int x, int y, char color)
{
	x += 2; // there's a portion of the screen that is not visible.

	*(COLORRAM + SCREEN_HEIGHT*(x + 1) - y - 1) = color;
}

#define HIGHLIGHT_CELL_COLOR 4
#define GRID_COLOR 14
void highlight_cell(minefield* mf, int x, int y)
{
    UNUSED(mf)
#if 0
//Experimental:
	char color = HIGHLIGHT_CELL_COLOR;
	set_color(x-1, y-1, color);
	set_color( x,  y-1, color);
	set_color(x+1, y-1, color);

	set_color(x-1,  y, color);
	set_color(x+1,  y, color);

	set_color(x-1, y+1, color);
	set_color( x,  y+1, color);
	set_color(x+1, y+1, color);
#else
    UNUSED(x)
    UNUSED(y)
#endif
}

uint8_t get_char(int x, int y)
{
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
		set_color(x, y, color);
		set_tile(x, y++, *(ptr++) - 55);
	}
}

void clear_screen(){
	for (int i=0; i<1024; i++){
		*(VIDEORAM+i) = BLANK;
		*(COLORRAM+i) = CLEAR_COLOR;
	}
}

void enable_chars(){
	// a.k.a. "Foreground Tiles"
	*HWCFG |= (1 << 7);
}

void init_video(){
	*VIDEOCFG = 0x30; // enables bg / enables sprites / selects sprite3bank #0
	*HWCFG = 0x00; // screen is not flipped and chars are initially disabled

	enable_chars();
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


void blink_cursor(minefield* mf){
	uint8_t x = mf->current_cell % mf->width;
	uint8_t y = mf->current_cell / mf->width;
	uint8_t tile = get_tile(MINEFIELD_X_OFFSET + x*2 + 1,
			  				MINEFIELD_Y_OFFSET + y*2 + 1);
	set_color(MINEFIELD_X_OFFSET + x*2 + 1,
			  MINEFIELD_Y_OFFSET + y*2 + 1,
			  ((count_loops / 120) % 2) ? BLINK_COLOR : get_tile_color(tile));
}


void platform_shutdown()
{
}
