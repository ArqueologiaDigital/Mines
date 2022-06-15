#include "mines.xpm"
#include "video-tiles.h"
#include <stdio.h>
#include <stdlib.h>

#define WIDTH 320
FILE *fp;

void save_tile(int x, int y, int tile_number){
	int address = 16 * tile_number;

	fseek(fp, address, SEEK_SET);
	for (int i=0; i<8; i++){
		char value = 0b11110000;
		for (int j=0; j<4; j++){
			if(mines[3 + y*8 + 7 - j - 4][x*8 + 7 - i] == ' '){
				value |= (1 << j);
				value &= ~(1 << (j+4));
			}
		}
		fwrite(&value, 1, 1, fp);

		value = 0b11110000;
		for (int j=0; j<4; j++){
			if(mines[3 + y*8 + 7 - j][x*8 + 7 - i] == ' '){
				value |= (1 << j);
				value &= ~(1 << (j+4));
			}
		}
		fwrite(&value, 1, 1, fp);
	}
}

int main() {
	fp = fopen("./build/gs01.11f", "r+");
	if (fp == NULL) {
		fprintf(stderr, "./build/gs01.11f: file not found\n");
		exit(-1);
	}

	save_tile(0, 0, ONE_BOMB);
	save_tile(1, 0, TWO_BOMBS);
	save_tile(2, 0, THREE_BOMBS);
	save_tile(3, 0, FOUR_BOMBS);
	save_tile(4, 0, FIVE_BOMBS);
	save_tile(5, 0, SIX_BOMBS);
	save_tile(6, 0, SEVEN_BOMBS);
	save_tile(7, 0, EIGHT_BOMBS);

	save_tile(0, 1, BOMB);
	save_tile(1, 1, FLAG);
	save_tile(2, 1, QUESTION_MARK);
	save_tile(3, 1, BLANK);
	save_tile(3, 1, GROUND);

	save_tile(0, 2, MINEFIELD_CORNER_TOP_LEFT);
	save_tile(1, 2, MINEFIELD_TOP_TEE);
	save_tile(2, 2, MINEFIELD_HORIZONTAL_TOP);
	save_tile(2, 2, MINEFIELD_HORIZONTAL_MIDDLE);
	save_tile(2, 2, MINEFIELD_HORIZONTAL_BOTTOM);
	save_tile(4, 2, MINEFIELD_CORNER_TOP_RIGHT);

	save_tile(0, 3, MINEFIELD_LEFT_TEE);
	save_tile(1, 3, MINEFIELD_CROSS);
	save_tile(4, 3, MINEFIELD_RIGHT_TEE);

	save_tile(0, 4, MINEFIELD_VERTICAL_LEFT);
	save_tile(0, 4, MINEFIELD_VERTICAL_MIDDLE);
	save_tile(0, 4, MINEFIELD_VERTICAL_RIGHT);
	save_tile(2, 4, CLOSED_CELL);

	save_tile(0, 5, MINEFIELD_CORNER_BOTTOM_LEFT);
	save_tile(1, 5, MINEFIELD_BOTTOM_TEE);
	save_tile(4, 5, MINEFIELD_CORNER_BOTTOM_RIGHT);

	save_tile(7, 2, CORNER_TOP_LEFT);
	save_tile(10, 2, CORNER_TOP_RIGHT);
	save_tile(7, 5, CORNER_BOTTOM_LEFT);
	save_tile(10, 5, CORNER_BOTTOM_RIGHT);

	save_tile(8, 2, TOP_BORDER__LEFT);
	save_tile(9, 2, TOP_BORDER__RIGHT);
	save_tile(8, 5, BOTTOM_BORDER__LEFT);
	save_tile(9, 5, BOTTOM_BORDER__RIGHT);

	save_tile(7, 3, LEFT_BORDER__TOP);
	save_tile(7, 4, LEFT_BORDER__BOTTOM);
	save_tile(10, 3, RIGHT_BORDER__TOP);
	save_tile(10, 4, RIGHT_BORDER__BOTTOM);

	fclose(fp);
	return 0;
}

