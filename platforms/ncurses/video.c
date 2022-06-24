#include "common.h"
#include "minefield.h"
#include "game.h"
#include "video-tiles.h"
#include "colors.h"
#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <assert.h>


#define NUM_TICKS 1


/* terminal size */
int rows;
int cols;


// TODO: perhaps part of this should move to a platform.c file
void platform_init()
{
    struct winsize size;
    if (ioctl(0, TIOCGWINSZ, (char *) &size) < 0) {
        rows = 80;
        cols = 25;
    }
    rows = size.ws_row;
    cols = size.ws_col;

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
    init_pair(BLANK_CELL_COLOR, COLOR_WHITE, COLOR_BLACK);
    init_pair(CLOSED_CELL_COLOR, COLOR_WHITE, COLOR_BLACK);
    init_pair(FLAG_COLOR, COLOR_WHITE, COLOR_RED);
    init_pair(QUESTION_MARK_COLOR, COLOR_RED, COLOR_WHITE);
    // disable waiting-for-input getch
    nodelay(stdscr, true);

    // Init random number generator:
    srand(time(NULL));
}


void set_tile(uint8_t x, uint8_t y, uint8_t tile)
{
    move(MINEFIELD_Y_OFFSET + y, MINEFIELD_X_OFFSET + x * 2);
    switch (tile) {
        case ONE_BOMB:
            attron(COLOR_PAIR(ONE_BOMB_COLOR));
            printw("1");
            break;
        case TWO_BOMBS:
            attron(COLOR_PAIR(TWO_BOMBS_COLOR));
            printw("2");
            break;
        case THREE_BOMBS:
            attron(COLOR_PAIR(THREE_BOMBS_COLOR));
            printw("3");
            break;
        case FOUR_BOMBS:
            attron(COLOR_PAIR(FOUR_BOMBS_COLOR));
            printw("4");
            break;
        case FIVE_BOMBS:
            attron(COLOR_PAIR(FIVE_BOMBS_COLOR));
            printw("5");
            break;
        case SIX_BOMBS:
            attron(COLOR_PAIR(SIX_BOMBS_COLOR));
            printw("6");
            break;
        case SEVEN_BOMBS:
            attron(COLOR_PAIR(SEVEN_BOMBS_COLOR));
            printw("7");
            break;
        case EIGHT_BOMBS:
            attron(COLOR_PAIR(EIGHT_BOMBS_COLOR));
            printw("8");
            break;
        case BLANK:
            attron(COLOR_PAIR(BLANK_CELL_COLOR));
            printw(" ");
            break;
        case BOMB:
            attron(COLOR_PAIR(UNCOVERED_BOMB_COLOR));
            printw("*");
            break;
        case FLAG:
            attron(COLOR_PAIR(FLAG_COLOR));
            printw("F");
            break;
        case QUESTION_MARK:
            attron(COLOR_PAIR(QUESTION_MARK_COLOR));
            printw("?");
            break;
        case CLOSED_CELL:
            attron(COLOR_PAIR(CLOSED_CELL_COLOR));
            printw("#");
            break;
        case EXPLOSION:
            attron(COLOR_PAIR(EXPLODING_BOMB_COLOR));
            printw("@");
            break;
        case MINEFIELD_HORIZONTAL_TOP:
        case MINEFIELD_HORIZONTAL_MIDDLE:
        case MINEFIELD_HORIZONTAL_BOTTOM:
        case MINEFIELD_TOP_TEE:
        case MINEFIELD_BOTTOM_TEE:
            attron(COLOR_PAIR(MINEFIELD_GRID_COLOR));
            printw("-");
            break;
        case MINEFIELD_CROSS:
        case MINEFIELD_CORNER_TOP_LEFT:
        case MINEFIELD_CORNER_TOP_RIGHT:
        case MINEFIELD_CORNER_BOTTOM_LEFT:
        case MINEFIELD_CORNER_BOTTOM_RIGHT:
            attron(COLOR_PAIR(MINEFIELD_GRID_COLOR));
            printw("+");
            break;
        case MINEFIELD_VERTICAL_LEFT:
        case MINEFIELD_LEFT_TEE:
        case MINEFIELD_RIGHT_TEE:
        case MINEFIELD_VERTICAL_MIDDLE:
        case MINEFIELD_VERTICAL_RIGHT:
            attron(COLOR_PAIR(MINEFIELD_GRID_COLOR));
            printw("|");
            break;
        default:
            fprintf(stderr, "\n%i\n", tile);
            assert(false);
    }
}


void put_cursor(uint8_t x, uint8_t y)
{
    move(MINEFIELD_Y_OFFSET + y, MINEFIELD_X_OFFSET + x * 2 - 1);
    chtype type = inch();
    chtype colors = type & A_COLOR;
    unsigned char ch = type & A_CHARTEXT;

    attron(colors & A_STANDOUT);
    printw("%c", ch);
    standend();
}


void hide_cursor(uint8_t x, uint8_t y)
{
    move(MINEFIELD_Y_OFFSET + y, MINEFIELD_X_OFFSET + x * 2);
    chtype type = inch();
    uint8_t colors = type & A_COLOR;
    unsigned char ch = type & A_CHARTEXT;
    attron(colors & ~(A_STANDOUT));
    printw("%c", ch);
}


void highlight_current_cell(minefield* mf)
{
    uint8_t x = CELL_X(mf, mf->current_cell) * 2 + MINEFIELD_X_OFFSET + 1;
    uint8_t y = CELL_Y(mf, mf->current_cell) * 2 + MINEFIELD_Y_OFFSET + 1;

    if (mf->state == PLAYING_GAME) {
        put_cursor(x, y);
    } else if (mf->state == GAME_OVER) {
        set_tile(x, y, EXPLOSION);
        hide_cursor(x, y);
    } else {
        hide_cursor(x, y);
    }
}


void draw_title_screen(minefield* mf)
{
    move(1,4);
    attron(COLOR_PAIR(TEXT_COLOR));
    printw("Mines!");

    move(MINEFIELD_Y_OFFSET + mf->height * 2 + 2, 1);
    printw("         ");

    move(MINEFIELD_Y_OFFSET + mf->height * 2 + 3, 1);
    printw("Press any key to start!             ");
    move(MINEFIELD_Y_OFFSET + mf->height * 2 + 3, 24);

    refresh();
}


void draw_game_over(minefield* mf)
{
    draw_minefield(mf);

    move(MINEFIELD_Y_OFFSET + mf->height * 2 + 2, 1);
    attron(COLOR_PAIR(TEXT_COLOR));
    if (mf->state == GAME_OVER)
        printw("GAME OVER");
    else
        printw("GAME WON!");

    move(MINEFIELD_Y_OFFSET + mf->height * 2 + 3, 1);
    attron(COLOR_PAIR(TEXT_COLOR));
    printw("Press button to restart or Q to exit");

    refresh();
}


void update_fps(float fps)
{
    char s[20];
    snprintf(s, 20, "%.2f fps", fps);
    move(0, cols - strlen(s));
    printw(s);

    refresh();
}


void wait_tick()
{
    static int frames = 0;
    static int total = 0;
    static float fps = 0.0;
    // update screen 60 times per second
    static double interval = NUM_TICKS * 1000 / 60;
    static clock_t last_time = 0;
    clock_t current = clock();
    long elapsed = current - last_time;

    // wait the remaining time in the frame
    usleep(interval > elapsed ? interval - elapsed : 0);

    frames++;
    total += interval > elapsed ? interval : elapsed;

    if (total >= 1000) {
        fps = frames * ((float) 1000) / total;
        total -= 1000;
        update_fps(fps);
        frames = 0;
    }

    last_time = current;
}


void idle_update(minefield* mf)
{
    // update everything first, then wait for the next frame
    wait_tick();
}


void platform_shutdown()
{
    endwin();
    exit(0);
}
