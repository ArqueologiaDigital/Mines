#include "../../common/minefield.h"
#include <ncurses.h>
#include <stdlib.h>
#include <time.h>

// TODO: perhaps part of this should move to a platform.c file
void platform_init()
{
	// Initialize ncurses library and setup colors
    initscr();
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLUE);
    init_pair(2, COLOR_BLUE, COLOR_WHITE);
    init_pair(3, COLOR_RED, COLOR_WHITE);

    bkgd(COLOR_PAIR(1));
    attron(COLOR_PAIR(3));

	// Init random number generator:
    srand(time(NULL));
}

void draw_minefield(minefield* mf){
    move(2,1);
    printw("Mines!");
    attroff(COLOR_PAIR(3));
    attron(COLOR_PAIR(2));

	for (uint8 x = 0; x < mf->width; x++){
		for (uint8 y = 0; y < mf->height; y++){
		    move(5 + y*2, 5 + x*2);
			if (CELL(mf, x, y) & HASBOMB){
				printw("*");
			} else {
				uint8 count = CELL(mf, x, y) & 0x0F;
				if (count > 0) printw("%d", count);
			}
		}
	}

    move(mf->height*2 + 8, 1);
    printw("Qualquer tecla para sair.");
    attroff(COLOR_PAIR(2));
    refresh();
}

void shutdown()
{
    endwin();
    exit(0);  
}
