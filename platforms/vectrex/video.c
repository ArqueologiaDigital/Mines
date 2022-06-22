#include "video-tiles.h"
#include <time.h>


static char* tile_text[MAX_VIDEO_TILES];

void set_tile(uint8_t dst_x, uint8_t dst_y, uint8_t tile)
{
//    print_str_c( 0x10 + dst_x, -0x50 + dst_y, tile_text[tile]);
}

void highlight_cell(minefield *mf, int x, int y)
{
    if (mf->state == GAME_OVER)
        set_tile(x, y, EXPLOSION);
    else
        set_tile(x, y, CURSOR);
}


void video_init(){
	tile_text[ONE_BOMB] = "1";
	tile_text[TWO_BOMBS] = "2";
	tile_text[THREE_BOMBS] = "3";
	tile_text[FOUR_BOMBS] = "4";
	tile_text[FIVE_BOMBS] = "5";
	tile_text[SIX_BOMBS] = "6";
	tile_text[SEVEN_BOMBS] = "7";
	tile_text[EIGHT_BOMBS] = "8";
	tile_text[BLANK] = " ";
	tile_text[CURSOR] = "C";

	tile_text[BOMB] = "*";
	tile_text[FLAG] = "F";
	tile_text[QUESTION_MARK] = "?";
	tile_text[EXPLOSION] = "X";

	tile_text[MINEFIELD_CORNER_TOP_LEFT] = ".";
	tile_text[MINEFIELD_TOP_TEE] = ".";
	tile_text[MINEFIELD_HORIZONTAL_TOP] = ".";
	tile_text[MINEFIELD_HORIZONTAL_MIDDLE] = ".";
	tile_text[MINEFIELD_HORIZONTAL_BOTTOM] = ".";
	tile_text[MINEFIELD_CORNER_TOP_RIGHT] = ".";

	tile_text[MINEFIELD_LEFT_TEE] = ".";
	tile_text[MINEFIELD_CROSS] = ".";
	tile_text[MINEFIELD_RIGHT_TEE] = ".";

	tile_text[MINEFIELD_VERTICAL_LEFT] = ".";
	tile_text[MINEFIELD_VERTICAL_MIDDLE] = ".";
	tile_text[MINEFIELD_VERTICAL_RIGHT] = ".";
	tile_text[CLOSED_CELL] = "-";

	tile_text[MINEFIELD_CORNER_BOTTOM_LEFT] = ".";
	tile_text[MINEFIELD_BOTTOM_TEE] = ".";
	tile_text[MINEFIELD_CORNER_BOTTOM_RIGHT] = ".";

	tile_text[CORNER_TOP_LEFT] = "~";
	tile_text[CORNER_TOP_RIGHT] = "~";
	tile_text[CORNER_BOTTOM_LEFT] = "~";
	tile_text[CORNER_BOTTOM_RIGHT] = "~";

	tile_text[TOP_BORDER__LEFT] = "~";
	tile_text[TOP_BORDER__RIGHT] = "~";
	tile_text[BOTTOM_BORDER__LEFT] = "~";
	tile_text[BOTTOM_BORDER__RIGHT] = "~";

	tile_text[LEFT_BORDER__TOP] = "~";
	tile_text[LEFT_BORDER__BOTTOM] = "~";
	tile_text[RIGHT_BORDER__TOP] = "~";
	tile_text[RIGHT_BORDER__BOTTOM] = "~";
}

void platform_init()
{
    // Init random number generator:
    srand(time(NULL));

    video_init();
}

extern char joy;
extern char buttons;
void draw_minefield_contents(minefield* mf);
void idle_update(minefield *mf) {
    //wait_retrace();
    
    //joy = read_joystick(1);
    //buttons = read_buttons();
    
    //intensity(0x7f);
    
    draw_minefield_contents(mf);
}


void highlight_current_cell(minefield* mf)
{
}


void platform_shutdown()
{
	// TODO: how to reboot the console or at least to restart the game?
    exit(0);
}
