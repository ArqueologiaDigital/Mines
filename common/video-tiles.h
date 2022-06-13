#ifndef VIDEO_TILES_H
#define VIDEO_TILES_H

#include <stdint.h>
#include "minefield.h"

void set_tile(uint8_t dst_x, uint8_t dst_y, uint8_t tile);
void draw_scenario();
void highlight_cell(minefield* mf, int x, int y);

enum {
	/*	NOTE:
		The tile codes for the number
		of bombs *MUST* be sequential
		because the video code assumes
		that to be the case.	*/
    ONE_BOMB = 0,
    TWO_BOMBS = 1,
    THREE_BOMBS = 2,
    FOUR_BOMBS = 3,
    FIVE_BOMBS = 4,
    SIX_BOMBS = 5,
    SEVEN_BOMBS = 6,
    EIGHT_BOMBS = 7,
    
    BLANK = 8,
    CURSOR = 9,
    BOMB = 12,
    FLAG = 13,
    QUESTION_MARK = 14,
    EXPLOSION = 15,
    GROUND = 16,
    MINEFIELD_CORNER_TOP_LEFT = 24,
    MINEFIELD_TOP_TEE = 25,
    MINEFIELD_HORIZONTAL_TOP = 26,
    MINEFIELD_CORNER_TOP_RIGHT = 28,

    CORNER_TOP_LEFT = 31,
    TOP_BORDER__LEFT = 32,
    TOP_BORDER__RIGHT = 33,
    CORNER_TOP_RIGHT = 34,
    LEFT_BORDER__TOP = 43,
    RIGHT_BORDER__TOP = 46,
    LEFT_BORDER__BOTTOM = 55,
    RIGHT_BORDER__BOTTOM = 58,
    CORNER_BOTTOM_LEFT = 67,
    BOTTOM_BORDER__LEFT = 68,
    BOTTOM_BORDER__RIGHT = 69,
    CORNER_BOTTOM_RIGHT = 70,

    MINEFIELD_LEFT_TEE = 36,
    MINEFIELD_CROSS = 37,
    MINEFIELD_HORIZONTAL_MIDDLE = 38,
    MINEFIELD_VERTICAL_MIDDLE = 39,
    MINEFIELD_RIGHT_TEE = 40,
    MINEFIELD_VERTICAL_LEFT = 48,
    CLOSED_CELL = 50,
    MINEFIELD_VERTICAL_RIGHT = 52,
    MINEFIELD_CORNER_BOTTOM_LEFT = 60,
    MINEFIELD_BOTTOM_TEE = 61,
    MINEFIELD_HORIZONTAL_BOTTOM = 63,
    MINEFIELD_CORNER_BOTTOM_RIGHT = 64,
};

#endif /* VIDEO_TILES_H */
