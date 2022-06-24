#ifndef VIDEO_TILES_H
#define VIDEO_TILES_H


#include <stdint.h>
#include "minefield.h"


/**
 * [Required] Set the tile position at the (`x`, `y`) coordinates with `tile`
 * index.
 *
 * Implementation details
 * ----------------------
 *
 * See [tile_index](#tile_index) for tile index values.
 */
void set_tile(uint8_t x, uint8_t y, uint8_t tile);


/**
 * [Provided] Draws the cell at (`x`, `y`) board coordinates on the screen.
 *
 * Implementation details
 * ----------------------
 *
 * See [tile_index](#tile_index) for tile index values.
 */
void draw_single_cell(minefield* mf, uint8_t x, uint8_t y);


/**
 * [Provided] Draw the background image that surrounds the board.
 *
 * Note: some platforms use the `GROUND` tile to cover the whole background
 * area.
 *
 * See [tile_index](#tile_index) for the index value of the `GROUND` tile.
 */
void draw_scenario();


/**
 * [Required] Draw a tile or sprite cursor on the board.
 *
 * Implementation details
 * ----------------------
 *
 * If game status is `GAME_OVER`, the current cursor should be replaced by
 * (or drawn together with) an `EXPLOSION` tile.
 *
 * See [tile_index](#tile_index) for the index value of the `EXPLOSION` tile.
 */
void highlight_current_cell(minefield* mf);


enum {
    /*    NOTE:
        The tile codes for the number
        of bombs *MUST* be sequential
        because the video code assumes
        that to be the case.    */
    ONE_BOMB,
    TWO_BOMBS,
    THREE_BOMBS,
    FOUR_BOMBS,
    FIVE_BOMBS,
    SIX_BOMBS,
    SEVEN_BOMBS,
    EIGHT_BOMBS,
    
    BLANK,
    CURSOR,
    BOMB,
    FLAG,
    QUESTION_MARK,
    EXPLOSION,
    GROUND,
    MINEFIELD_CORNER_TOP_LEFT,
    MINEFIELD_TOP_TEE,
    MINEFIELD_HORIZONTAL_TOP,
    MINEFIELD_CORNER_TOP_RIGHT,

    CORNER_TOP_LEFT,
    TOP_BORDER__LEFT,
    TOP_BORDER__RIGHT,
    CORNER_TOP_RIGHT,
    LEFT_BORDER__TOP,
    RIGHT_BORDER__TOP,
    LEFT_BORDER__BOTTOM,
    RIGHT_BORDER__BOTTOM,
    CORNER_BOTTOM_LEFT,
    BOTTOM_BORDER__LEFT,
    BOTTOM_BORDER__RIGHT,
    CORNER_BOTTOM_RIGHT,

    MINEFIELD_LEFT_TEE,
    MINEFIELD_CROSS,
    MINEFIELD_HORIZONTAL_MIDDLE,
    MINEFIELD_VERTICAL_MIDDLE,
    MINEFIELD_RIGHT_TEE,
    MINEFIELD_VERTICAL_LEFT,
    CLOSED_CELL,
    MINEFIELD_VERTICAL_RIGHT,
    MINEFIELD_CORNER_BOTTOM_LEFT,
    MINEFIELD_BOTTOM_TEE,
    MINEFIELD_HORIZONTAL_BOTTOM,
    MINEFIELD_CORNER_BOTTOM_RIGHT,

    /* Insert new tiles here */

    MAX_VIDEO_TILES,
};

#endif /* VIDEO_TILES_H */
