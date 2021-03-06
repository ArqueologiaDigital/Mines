#include <stdio.h>
#include <sys/types.h>
#include <psxetc.h>
#include <psxgte.h>
#include <psxgpu.h>
#include "../../common/video-tiles.h"
#include "../../common/common.h"
#include "../../common/main.h"

#define OTLEN 8
#define TILESIZE 10
#define TILESKIP 9
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

DISPENV disp[2];
DRAWENV draw[2];
int db;

u_long ot[2][OTLEN];
char pribuff[2][32768];
char *nextpri = pribuff[0];

void init_gl() {
    ResetGraph(0);
    
    SetDefDispEnv(&disp[0], 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    SetDefDispEnv(&disp[1], 0, SCREEN_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT);
    
    SetDefDrawEnv(&draw[0], 0, SCREEN_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT);
    SetDefDrawEnv(&draw[1], 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    
    SetDispMask(1);
    setRGB0(&draw[0], 0, 0, 96);
    setRGB0(&draw[1], 0, 0, 96);
    draw[0].isbg = 1;
    draw[1].isbg = 1;

    PutDispEnv(&disp[db]);
    PutDrawEnv(&draw[db]);

	FntLoad(960, 0);
	FntOpen(0, 8, 320, 224, 0, 100);
}

void display()
{
    DrawSync(0);
    VSync(0);

    PutDispEnv(&disp[db]);
    PutDrawEnv(&draw[db]);

    DrawOTag(ot[db][OTLEN - 1]);

    db = !db;

    nextpri = pribuff[db];
}

void set_tile_color(TILE *tile, uint8_t tile_type) {
    switch (tile_type) {
        case MINEFIELD_CORNER_TOP_LEFT: 
        case MINEFIELD_CORNER_TOP_RIGHT:
        case MINEFIELD_CORNER_BOTTOM_LEFT:
        case MINEFIELD_CORNER_BOTTOM_RIGHT:
            setRGB0(tile, 0, 63, 0);
            break;
        case MINEFIELD_TOP_TEE:
        case MINEFIELD_BOTTOM_TEE:
        case MINEFIELD_LEFT_TEE:
        case MINEFIELD_RIGHT_TEE:
            setRGB0(tile, 0, 63, 0);
            break;
        case MINEFIELD_VERTICAL_LEFT:
        case MINEFIELD_VERTICAL_MIDDLE:
        case MINEFIELD_VERTICAL_RIGHT:
            setRGB0(tile, 0, 63, 0);
            break;
        case MINEFIELD_HORIZONTAL_TOP:
        case MINEFIELD_HORIZONTAL_MIDDLE:
        case MINEFIELD_HORIZONTAL_BOTTOM:
        case MINEFIELD_CROSS:
            setRGB0(tile, 0, 63, 0);
            break;
        case CURSOR:
            setRGB0(tile, 255, 255, 0);
            break;
        default:
            setRGB0(tile, 31, 128, 0);
            break;
    }
}

void set_tile(uint8_t x, uint8_t y, uint8_t tile_type) {
    // needed on real hw but we don't have to clean the whole ot
    ClearOTagR(ot[db], OTLEN - 4);
    TILE *tile;
    tile = (TILE*)nextpri;

    setTile(tile);
    setXY0(tile, x * TILESKIP, y * TILESKIP);
    setWH(tile, TILESIZE, TILESIZE);
    set_tile_color(tile, tile_type);
    if (tile_type == CURSOR)
        // draw the cursor on the first layer
        addPrim(&ot[db][OTLEN - OTLEN + 1], tile);
    else
        // draw the minefield as the last layer, way up the z axis 
        addPrim(&ot[db][OTLEN - 1], tile);

    nextpri += sizeof(TILE);
}

void highlight_current_cell(minefield* mf) {
    uint8_t x = CELL_X(mf, mf->current_cell);
    uint8_t x2 = x * 2 + MINEFIELD_X_OFFSET + 1;
    uint8_t y = CELL_Y(mf, mf->current_cell);
    uint8_t y2 = y * 2 + MINEFIELD_Y_OFFSET + 1;
    static uint8_t old_x = 0;
    static uint8_t old_y = 0;

    // remove old cursor position
    //draw_single_cell(mf, old_x, old_y);

    if (mf->state == GAME_OVER)
        set_tile(x2, y2, EXPLOSION);
    else
        set_tile(x2, y2, CURSOR);

    old_x = x;
    old_y = y;
}

void idle_update(minefield* mf) {}

void platform_main_loop(minefield* mf) {
    bool shouldCallDisplay = true;
    while (mf->state != QUIT) {
        switch (mf->state)
        {
            case TITLE_SCREEN:
                title_screen_update(mf);
                shouldCallDisplay = false;
                break;
            case PLAYING_GAME:
                gameplay_update(mf);
                shouldCallDisplay = true;
                break;
            case GAME_WON:
            case GAME_OVER:
                game_over_update(mf);
                break;
        }

        if (shouldCallDisplay) {
            display();
        }
        idle_update(mf); // useful for doing other things
                         // such as running animations
    }
}

void draw_title_screen(minefield* mf) {
    // Need this here on real hw
    ClearOTagR(ot[db], OTLEN);
    FntPrint(-1, "MINESWEEPER\n");
    FntPrint(-1, "BROUGHT TO YOU BY ARQUEOLOGIA DIGITAL\n");
    FntPrint(-1, "PRESS START\n");
    FntFlush(-1);
    display();  
}
