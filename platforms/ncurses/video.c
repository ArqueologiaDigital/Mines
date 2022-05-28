#include <ncurses.h>
#include <stdlib.h>

void platform_init()
{
    initscr();
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLUE);
    init_pair(2, COLOR_BLUE, COLOR_WHITE);
    init_pair(3, COLOR_RED, COLOR_WHITE);

    bkgd(COLOR_PAIR(1));
    attron(COLOR_PAIR(3));
}

void draw_minefield(){
	// FIXME!
	// Atualizar essa implementacao para de fato desenhar um campo de jogo
    move(2,1);
    printw("Olá mundo!");
    attroff(COLOR_PAIR(3));
    attron(COLOR_PAIR(2));
    move(3,1);
    printw("Qualquer tecla para sair.");
    attroff(COLOR_PAIR(2));
    refresh();
}

void shutdown()
{
    endwin();
    exit(0);  
}
