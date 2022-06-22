#include "common.h"
#include "minefield.h"
#include "game.h"
#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>

enum {
	ONE_BOMB_COLOR = 1,
	TWO_BOMBS_COLOR = 2,
	THREE_BOMBS_COLOR = 3,
	FOUR_BOMBS_COLOR = 4,
	FIVE_BOMBS_COLOR = 5,
	SIX_BOMBS_COLOR = 6,
	SEVEN_BOMBS_COLOR = 7,
	EIGHT_BOMBS_COLOR = 8,
	UNCOVERED_BOMB_COLOR,
	EXPLODING_BOMB_COLOR,
	MINEFIELD_GRID_COLOR,
	TEXT_COLOR,
	CLOSED_CELL_COLOR,
	FLAG_COLOR,
	QUESTION_MARK_COLOR
};

// TODO: perhaps part of this should move to a platform.c file
void platform_init()
{
	// Initialize ncurses library and setup colors
	initscr();
	start_color();
	noecho();
	keypad(stdscr, TRUE);
	init_pair(ONE_BOMB_COLOR, COLOR_BLUE, COLOR_BLACK);
	init_pair(TWO_BOMBS_COLOR, COLOR_GREEN, COLOR_BLACK);
	init_pair(THREE_BOMBS_COLOR, COLOR_RED, COLOR_BLACK);
	init_pair(FOUR_BOMBS_COLOR, COLOR_BLUE, COLOR_BLACK);
	init_pair(FIVE_BOMBS_COLOR, COLOR_RED, COLOR_BLACK);
	init_pair(SIX_BOMBS_COLOR, COLOR_CYAN, COLOR_BLACK);
	init_pair(SEVEN_BOMBS_COLOR, COLOR_MAGENTA, COLOR_BLACK);
	init_pair(EIGHT_BOMBS_COLOR, COLOR_WHITE, COLOR_BLACK);
	init_pair(UNCOVERED_BOMB_COLOR, COLOR_BLACK, COLOR_YELLOW);
	init_pair(EXPLODING_BOMB_COLOR, COLOR_BLACK, COLOR_RED);
	init_pair(MINEFIELD_GRID_COLOR, COLOR_WHITE, COLOR_BLACK);
	init_pair(TEXT_COLOR, COLOR_WHITE, COLOR_BLACK);
	init_pair(CLOSED_CELL_COLOR, COLOR_WHITE, COLOR_BLACK);
	init_pair(FLAG_COLOR, COLOR_WHITE, COLOR_RED);
	init_pair(QUESTION_MARK_COLOR, COLOR_RED, COLOR_WHITE);

	// Init random number generator:
	srand(time(NULL));
}

#define minefield_x_position 5
#define minefield_y_position 5

void draw_minefield_contents(minefield* mf)
{
	for (uint8_t x = 0; x < mf->width; x++) {
		for (uint8_t y = 0; y < mf->height; y++){
			move(minefield_y_position + y * 2, minefield_x_position + x * 2);
			if (CELL_INDEX(mf, x, y) == mf->current_cell)
				standout();

			if (CELL(mf, x, y) & ISOPEN) {
				if (CELL(mf, x, y) & HASBOMB) {
					attron(COLOR_PAIR(UNCOVERED_BOMB_COLOR));
					printw("*");
				} else {
					uint8_t count = CELL(mf, x, y) & 0x0F;
					if (count > 0){
						attron(COLOR_PAIR(ONE_BOMB_COLOR + count - 1));
						printw("%d", count);
					} else {
						printw(" ");
					}
				}
			} else {
				if (CELL(mf, x, y) & HASFLAG){
					attron(COLOR_PAIR(FLAG_COLOR));
					printw("F");
				} else if (CELL(mf, x, y) & HASQUESTIONMARK){
					attron(COLOR_PAIR(QUESTION_MARK_COLOR));
					printw("?");
				} else {
					attron(COLOR_PAIR(CLOSED_CELL_COLOR));
					printw("#");
				}
			}

			if (CELL_INDEX(mf, x, y) == mf->current_cell)
				standend();
		}
	}

    refresh();
}

void draw_title_screen(minefield* mf)
{
    clear();

    move(1,4);
    attron(COLOR_PAIR(TEXT_COLOR));
    printw("Mines!");

    move(minefield_y_position + mf->height * 2 + 2, 1);
    printw("Press any key to start!");
    refresh();
}

void draw_game_over(minefield* mf)
{
    clear();
    draw_minefield(mf);

    move(minefield_y_position + mf->height * 2 + 2, 1);
    attron(COLOR_PAIR(TEXT_COLOR));
    printw("GAME OVER");

    move(minefield_y_position + mf->height * 2 + 3, 1);
    attron(COLOR_PAIR(TEXT_COLOR));
    printw("Press button to restart or Q to exit");

    refresh();
}

void draw_minefield(minefield* mf)
{
    clear();

    move(2,4);
    attron(COLOR_PAIR(TEXT_COLOR));
    printw("Mines!");

    move(minefield_y_position + mf->height * 2 + 3, 1);
    attron(COLOR_PAIR(TEXT_COLOR));
    printw("Press 'Q' to exit.");

    attron(COLOR_PAIR(MINEFIELD_GRID_COLOR));
	for (uint8_t x = 0; x <= mf->width; x++){
		for (uint8_t y = 0; y <= mf->height; y++){
		    move(minefield_y_position + y * 2, minefield_x_position + x * 2 - 1);
			if (y < mf->height) printw("|");
		    move(minefield_y_position + y * 2 - 1, minefield_x_position + x * 2 - 1);
			printw("+");
		    move(minefield_y_position + y * 2 - 1, minefield_x_position + x * 2);
			if (x < mf->width) printw("-");
		}
	}

    draw_minefield_contents(mf);
    refresh();
}

void idle_update(minefield* mf)
{
}

void platform_shutdown()
{
    endwin();
    exit(0);
}
