#ifndef MINEFIELD_H
#define MINEFIELD_H

#include <stdlib.h>
typedef unsigned char uint8;
#define CELL_INDEX(mf, x, y) (x + (y) * mf->width)
#define CELL(mf,x,y) mf->cells[CELL_INDEX(mf, x, y)]

// Note:
// The 4 least significant bits hold the computed number of
// neighbouring bombs around a given a cell.
#define HASBOMB (1 << 4)
#define ISOPEN (1 << 5)

typedef struct {
	uint8 width;
	uint8 height;
	uint8 current_cell;
	uint8* cells;
} minefield;

#define MINE_INPUT_IGNORED 0
#define MINE_INPUT_LEFT 1
#define MINE_INPUT_RIGHT 2
#define MINE_INPUT_UP 3
#define MINE_INPUT_DOWN 4
#define MINE_INPUT_OPEN 5

void setup_minefield(minefield* mf, uint8 width, uint8 height, uint8 num_bombs);

#endif //#define MINEFIELD_H
