#ifndef MINEFIELD_H
#define MINEFIELD_H

#include <stdlib.h>
typedef unsigned char uint8;
#define CELL(mf,x,y) mf->cells[x + (y) * mf->width]

// Note:
// The 4 least significant bits hold the computed number of
// neighbouring bombs around a given a cell.
#define HASBOMB (1 << 4)
#define ISOPEN (1 << 5)

typedef struct {
      uint8 width;
      uint8 height;
      uint8* cells;
} minefield;

void setup_minefield(minefield* mf, uint8 width, uint8 height, uint8 num_bombs);

#endif //#define MINEFIELD_H
