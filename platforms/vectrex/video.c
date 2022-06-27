#include "video-tiles.h"
#include <time.h>


//static char* tile_text[MAX_VIDEO_TILES];
char closed_tile[] = {
    4,
    0, 0,
    0, 10,
    10, 10,
    10, 10
};

void vectrex_bios_Draw_VLc(uint16_t vector_list)
{
/*
;-----------------------------------------------------------------------;
;       F3CE    Draw_VLc                                                ;
;                                                                       ;
; This routine draws vectors between the set of (y,x) points pointed    ;
; to by the X register.  The number of vectors to draw is specified     ;
; as the first byte in the vector list.  The current scale factor is    ;
; used.  The vector list has the following format:                      ;
;                                                                       ;
;       count, rel y, rel x, rel y, rel x, ...                          ;
;                                                                       ;
; ENTRY DP = $D0                                                        ;
;       X-reg points to the vector list                                 ;
;                                                                       ;
; EXIT: X-reg points to next byte after list                            ;
;                                                                       ;
;       D-reg trashed                                                   ;
;-----------------------------------------------------------------------;
*/
    __asm__ __volatile("ldx %0\n\t"
                       "jsr 0xf3ce"
                       : /* no output */
                       : "m" (vector_list)
                       : "d");
}

inline uint8_t vectrex_bios_print_str(){
    // This routine generates a random 1-byte number,
    // and places it in the A register.
    register uint8_t number __asm__("a");
    __asm__ __volatile("jsr 0xf517");
    return number;
}

void set_tile(uint8_t dst_x, uint8_t dst_y, uint8_t tile)
{
	switch(tile){
		case CLOSED_CELL:
			vectrex_bios_Draw_VLc((uint16_t) &closed_tile);
			break;
		default:
			break;
	}
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
#if 0
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
#endif
}


void platform_init()
{
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
