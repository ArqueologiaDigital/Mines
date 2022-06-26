#include <stdio.h>
#include <sys/types.h>
#include <psxetc.h>
#include <psxgte.h>
#include <psxgpu.h>
#include "../../common/video-tiles.h"

#define OTLEN 8

// Define display/draw environments for double buffering
DISPENV disp[2];
DRAWENV draw[2];
int db;

u_long ot[2][OTLEN];
char pribuff[2][32768];
char *nextpri;

void init_gl() {
    // This not only resets the GPU but it also installs the library's
    // ISR subsystem to the kernel
    ResetGraph(0);
    
    // Define display environments, first on top and second on bottom
    SetDefDispEnv(&disp[0], 0, 0, 320, 240);
    SetDefDispEnv(&disp[1], 0, 240, 320, 240);
    
    // Define drawing environments, first on bottom and second on top
    SetDefDrawEnv(&draw[0], 0, 240, 320, 240);
    SetDefDrawEnv(&draw[1], 0, 0, 320, 240);
    
    // Set and enable clear color
    setRGB0(&draw[0], 0, 96, 0);
    setRGB0(&draw[1], 0, 96, 0);
    draw[0].isbg = 1;
    draw[1].isbg = 1;
    
    // Clear double buffer counter
    db = 0;
    
    // Apply the GPU environments
    PutDispEnv(&disp[db]);
    PutDrawEnv(&draw[db]);

    // Load test font
	FntLoad(960, 0);

	// Open up a test font text stream of 100 characters
	FntOpen(0, 8, 320, 224, 0, 100);
}

// Display function
void display()
{
    // Flip buffer index
    db = !db;
    
    // Wait for all drawing to complete
    DrawSync(0);
    
    // Wait for vertical sync to cap the logic to 60fps (or 50 in PAL mode)
    // and prevent screen tearing
    VSync(0);

    // Switch pages 
    PutDispEnv(&disp[db]);
    PutDrawEnv(&draw[db]);
    
    // Enable display output, ResetGraph() disables it by default
    SetDispMask(1);

    DrawOTag(ot[db]+OTLEN-1);
}

void platform_shutdown() {
}

void set_tile(uint8_t x, uint8_t y, uint8_t tile_uint) {
    TILE *tile;

    ClearOTagR(ot[db], OTLEN);
    tile = (TILE*)nextpri;

    setTile(tile);
    setXY0(tile, x, y);
    setWH(tile, 32, 32);
    setRGB0(tile, 255, 255, 0);

    addPrim(ot[db], tile);

    // TODO - sizeof seems to be crashing the emulator!
    //nextpri += sizeof(TILE);
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
