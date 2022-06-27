#ifndef VIDEO_TILES_H
#define VIDEO_TILES_H

#include <stdint.h>
#include "minefield.h"
#include "video-tile-codes.h"

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

#endif /* VIDEO_TILES_H */
