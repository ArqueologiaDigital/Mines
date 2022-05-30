#include "main.h"
#include "minefield.h"

void setup_minefield(minefield* mf, uint8 width, uint8 height, uint8 num_bombs){
	mf->width = width;
	mf->height = height;
        mf->current_cell = 0;
	mf->cells = malloc(width * height * sizeof(uint8));

	// Clear the minefield:
	for (uint8 x = 0; x < mf->width; x++){
		for (uint8 y = 0; y < mf->height; y++){
			CELL(mf, x, y) = 0;
		}
	}

	// Place bombs
	while (num_bombs--){
		uint8 x = random_number(0, mf->width - 1);
		uint8 y = random_number(0, mf->height - 1);
		CELL(mf, x, y) |= HASBOMB;
	}

	// Compute neighbouring bombs:
	for (uint8 x = 0; x < mf->width; x++){
		for (uint8 y = 0; y < mf->height; y++){
			if ((CELL(mf, x, y) & HASBOMB)) continue;

			uint8 count = 0;
			if (x > 0 && y > 0 && (CELL(mf, x-1, y-1) & HASBOMB)) count++;
			if (x > 0 && (CELL(mf, x-1, y) & HASBOMB)) count++;
			if (x > 0 && y < mf->height-1 && (CELL(mf, x-1, y+1) & HASBOMB)) count++;

			if (y > 0 && (CELL(mf, x, y-1) & HASBOMB)) count++;
			if (y < mf->height-1 && (CELL(mf, x, y+1) & HASBOMB)) count++;

			if (x < mf->width-1 && y > 0 && (CELL(mf, x+1, y-1) & HASBOMB)) count++;
			if (x < mf->width-1 && (CELL(mf, x+1, y) & HASBOMB)) count++;
			if (x < mf->width-1 && y < mf->height-1 && (CELL(mf, x+1, y+1) & HASBOMB)) count++;

			// Store the count in the 4 lowest bits:
			CELL(mf, x, y) |= count;
		}
	}
}
