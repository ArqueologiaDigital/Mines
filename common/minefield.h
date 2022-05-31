#ifndef MINEFIELD_H
#define MINEFIELD_H

#include <stdlib.h>
#include <stdint.h>

#define CELL_INDEX(mf, x, y) (x + (y) * mf->width)
#define CELL(mf,x,y) mf->cells[CELL_INDEX(mf, x, y)]

// Note:
// The 4 least significant bits hold the computed number of
// neighbouring bombs around a given a cell.
#define HASBOMB (1 << 4)
#define ISOPEN (1 << 5)
#define HASFLAG (1 << 6)

typedef struct {
	uint8_t width;
	uint8_t height;
	uint8_t current_cell;
	uint8_t* cells;
} minefield;

void setup_minefield(minefield* mf, uint8_t width, uint8_t height, uint8_t num_bombs);
void open_cell(minefield* mf, uint8_t x, uint8_t y);

#endif //#define MINEFIELD_H
