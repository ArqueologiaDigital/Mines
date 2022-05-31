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

#define MINE_INPUT_IGNORED 0
#define MINE_INPUT_LEFT 1
#define MINE_INPUT_RIGHT 2
#define MINE_INPUT_UP 3
#define MINE_INPUT_DOWN 4
#define MINE_INPUT_OPEN 5
#define MINE_INPUT_FLAG 6

void setup_minefield(minefield* mf, uint8_t width, uint8_t height, uint8_t num_bombs);
void open_cell(minefield* mf, uint8_t x, uint8_t y);

#endif //#define MINEFIELD_H
