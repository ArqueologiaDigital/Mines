#ifndef TILES_H
#define TILES_H

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


#ifdef _16X16_TILES
/**
 * [Optional] Set the block at the (`x`..`x+1`, `y`..`y+1`) coordinates with
 * a 16x16 `block` index. This is an alternative version of the board were
 * each cell is a 16x16 collection of pixels (or a group of 2x2 tiles).
 *
 * Implementation details
 * ----------------------
 *
 * _16X16_TILES must be defined to use 16x16 blocks of cells.
 */
void set_block(uint8_t x, uint8_t y, uint8_t block);
#endif /* _16X16_TILES */


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

#endif /* TILES_H */
