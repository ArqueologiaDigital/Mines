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
    noecho();
    keypad(stdscr, TRUE);
    init_pair(1, COLOR_BLUE, COLOR_BLACK);    // 1 bomb
    init_pair(2, COLOR_GREEN, COLOR_BLACK);   // 2 bombs
    init_pair(3, COLOR_RED, COLOR_BLACK);     // 3 bombs
    init_pair(4, COLOR_BLUE, COLOR_BLACK);    // 4 bombs
    init_pair(5, COLOR_RED, COLOR_BLACK);     // 5 bombs
    init_pair(6, COLOR_CYAN, COLOR_BLACK);    // 6 bombs
    init_pair(7, COLOR_MAGENTA, COLOR_BLACK); // 7 bombs
    init_pair(8, COLOR_WHITE, COLOR_BLACK);   // 8 bombs
    init_pair(9, COLOR_BLACK, COLOR_YELLOW);   // an uncovered bomb
    init_pair(10, COLOR_BLACK, COLOR_RED);    // an exploding bomb
    init_pair(11, COLOR_WHITE, COLOR_BLACK);    // the minefield grid

	// Init random number generator:
    srand(time(NULL));
}

void draw_minefield(minefield* mf){
    bkgd(COLOR_PAIR(1));
    attron(COLOR_PAIR(11));
    move(2,1);
    printw("Mines!");

    attron(COLOR_PAIR(11));
	for (uint8 x = 0; x <= mf->width; x++){
		for (uint8 y = 0; y <= mf->height; y++){
		    move(5 + y*2, 5 + x*2 - 1);
			if (y < mf->height) printw("|");
		    move(5 + y*2-1, 5 + x*2 - 1);
			printw("+");
		    move(5 + y*2-1, 5 + x*2);
			if (x < mf->width) printw("-");
		}
	}

	for (uint8 x = 0; x < mf->width; x++){
		for (uint8 y = 0; y < mf->height; y++){
		    move(5 + y*2, 5 + x*2);
                    if (CELL_INDEX(mf, x, y) == mf->current_cell) {
                      standout();
                    } else {
                      standend();
                    }
                    if (CELL(mf, x, y) & ISOPEN) {
			if (CELL(mf, x, y) & HASBOMB){
			    attron(COLOR_PAIR(9));
				printw("*");
			} else {
				uint8 count = CELL(mf, x, y) & 0x0F;
			    attron(COLOR_PAIR(1 + count));
				if (count > 0) printw("%d", count);
			}
                    } else {
                      attron(COLOR_PAIR(11));
                      printw("#");
                    }
		}
	}

    move(mf->height*2 + 8, 1);
    attron(COLOR_PAIR(11));
    printw("Pressione Ctrl+c para sair.");
    refresh();
}

void shutdown()
{
    endwin();
    exit(0);
}
