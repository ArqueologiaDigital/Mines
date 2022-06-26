#include <stdio.h>
#include <sys/types.h>
#include <psxetc.h>
#include <psxgte.h>
#include <psxgpu.h>
#include "../../common/video-tiles.h"
#include "../../common/common.h"

#define OTLEN 8

DISPENV disp[2];
DRAWENV draw[2];
int db;

u_long ot[2][OTLEN];
char pribuff[2][32768];
char *nextpri;
TILE *tile;

void init_gl() {
    ResetGraph(0);
    
    SetDefDispEnv(&disp[0], 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    SetDefDispEnv(&disp[1], 0, SCREEN_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT);
    
    SetDefDrawEnv(&draw[0], 0, SCREEN_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT);
    SetDefDrawEnv(&draw[1], 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    
    setRGB0(&draw[0], 0, 96, 0);
    setRGB0(&draw[1], 0, 96, 0);
    draw[0].isbg = 1;
    draw[1].isbg = 1;
    
    PutDispEnv(&disp[db]);
    PutDrawEnv(&draw[db]);

    // Load test font
	FntLoad(960, 0);

	// Open up a test font text stream of 100 characters
	FntOpen(0, 8, 320, 224, 0, 100);
}

void display()
{
    DrawSync(0);
    VSync(0);

    PutDispEnv(&disp[db]);
    PutDrawEnv(&draw[db]);
    
    SetDispMask(1);

    DrawOTag(ot[db] + OTLEN - 1);

    db = !db;
    nextpri = pribuff[db];
}

void set_tile(uint8_t x, uint8_t y, uint8_t tile_uint) {
    debug("x axis = ", x);
    debug("y axis = ", y);

    ClearOTagR(ot[db], OTLEN);
    tile = (TILE*)nextpri;

    setTile(tile);
    setXY0(tile, x , y);
    setWH(tile, 10, 10);
    setRGB0(tile, 255, 255, 0);

    addPrim(ot[db], tile);

    nextpri += sizeof(TILE);
    display();
}

void highlight_current_cell(minefield* mf) {}

void idle_update(minefield* mf) {}

void draw_title_screen(minefield* mf) {
    FntPrint(-1, "MINESWEEPER\n");
    FntPrint(-1, "BROUGHT TO YOU BY ARQUEOLOGIA DIGITAL\n");
    FntPrint(-1, "PRESS START\n");
    FntFlush(-1);
    display();  
}
