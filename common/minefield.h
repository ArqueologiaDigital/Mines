#ifndef MINEFIELD_H
#define MINEFIELD_H

#include <stdlib.h>
#include <stdint.h>

#define CELL_INDEX(mf, x, y)  ((x) + (y) * mf->width)
#define CELL(mf,x,y)          mf->cells[CELL_INDEX(mf, x, y)]
#define CURRENT_CELL_X(mf)    mf->current_cell % mf->width
#define CURRENT_CELL_Y(mf)    mf->current_cell / mf->width

/* game state */
enum {
	TITLE_SCREEN = 1,
	PLAYING_GAME = 2,
	GAME_OVER = 3,
	ENTER_HISCORE_SCREEN = 4,
	HIGHSCORES_LIST_SCREEN = 5,
	QUIT = 6
};

// Note:
// The 4 least significant bits hold the computed number of
// neighbouring bombs around a given a cell.
#define HASBOMB (1 << 4)
#define ISOPEN (1 << 5)
#define HASFLAG (1 << 6)
#define HASQUESTIONMARK (1 << 7)

typedef struct {
	uint8_t state;
	uint8_t width;
	uint8_t height;
	uint8_t current_cell;
	uint8_t* cells;
} minefield;

void open_cell(minefield* mf, uint8_t x, uint8_t y);
void open_block(minefield* mf, uint8_t x, uint8_t y);
uint8_t count_surrounding_flags(minefield* mf, uint8_t x, uint8_t y);

#endif //#define MINEFIELD_H
