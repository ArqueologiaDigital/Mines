#include "msx.h"

const uint8_t mines_palette[] = {
	0,  0,0,0, // black
	1,  0,0,0, // unused
	2,  0,0,0, // unused
	3,  0,0,0, // unused
	4,  1,3,2, // dark green
	5,  3,2,1, // brown
	6,  3,3,3, // dark grey
	7,  1,4,2, // light green
	8,  7,1,1, // red
	9,  1,2,7, // blue
	10, 7,4,1, // orange
	11, 7,1,4, // magenta
	12, 0,5,7, // cyan
	13, 5,5,5, // light grey
	14, 7,6,0, // yellow
	15, 7,7,7, // white
};

// Cinza claro: 6,6,6
