#include "video-tiles.h"
#include <mines.xpm>
#include <conio.h>
#include <dos.h>
#include <string.h>
#include <time.h>

#define SET_MODE 0x00
#define WRITE_DOT 0x0C
#define VIDEO_INT 0x10
#define VGA_TEXT_MODE 0x03
#define VGA_256_COLOR_MODE 0x13

void set_mode(unsigned char mode)
{
	union REGPACK regs;
	memset( &regs, 0, sizeof(union REGPACK) );
	regs.h.ah = SET_MODE;
	regs.h.al = mode;
	intr(VIDEO_INT, &regs);
}

uint8_t hex2int(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return 0xFF; // ERROR!
}

void set_palette() {
    uint8_t index, r, g, b;
    for (uint8_t i = 1; i <= 15; i++) {
        index = mines_xpm[i][0];
        r = (hex2int(mines_xpm[i][5]) << 4) | hex2int(mines_xpm[i][6]);
        g = (hex2int(mines_xpm[i][7]) << 4) | hex2int(mines_xpm[i][8]);
        b = (hex2int(mines_xpm[i][9]) << 4) | hex2int(mines_xpm[i][10]);

		r = (uint8_t) ((double) r) * 7 / 0xff;
		g = (uint8_t) ((double) g) * 7 / 0xff;
		b = (uint8_t) ((double) b) * 7 / 0xff;

        outp(0x3c8, index);
        outp(0x3c9, r << 3);
        outp(0x3c9, g << 3);
        outp(0x3c9, b << 3);
    }
}

void video_init(){
	set_mode(VGA_256_COLOR_MODE);
	set_palette();
}

void plot_pixel_slow(int x,int y,uint8_t color)
{
  union REGPACK regs;

  regs.h.ah = WRITE_DOT;
  regs.h.al = color;
  regs.x.cx = x;
  regs.x.dx = y;
  intr(VIDEO_INT, &regs);
}

#define TILE_OFFSET(x, y) (((y) << 8) | (x))
uint16_t get_tile_offset(uint8_t tile){
	switch(tile){
		case ONE_BOMB: return TILE_OFFSET(0, 0);
		case TWO_BOMBS: return TILE_OFFSET(1, 0);
		case THREE_BOMBS: return TILE_OFFSET(2, 0);
		case FOUR_BOMBS: return TILE_OFFSET(3, 0);
		case FIVE_BOMBS: return TILE_OFFSET(4, 0);
		case SIX_BOMBS: return TILE_OFFSET(5, 0);
		case SEVEN_BOMBS: return TILE_OFFSET(6, 0);
		case EIGHT_BOMBS: return TILE_OFFSET(7, 0);
		case BLANK: return TILE_OFFSET(8, 0);
		case CURSOR: return TILE_OFFSET(9, 0);

		case BOMB: return TILE_OFFSET(0, 1);
		case FLAG: return TILE_OFFSET(1, 1);
		case QUESTION_MARK: return TILE_OFFSET(2, 1);
		case EXPLOSION: return TILE_OFFSET(3, 1);
		case GROUND: return TILE_OFFSET(4, 1);

		case MINEFIELD_CORNER_TOP_LEFT: return TILE_OFFSET(0, 2);
		case MINEFIELD_TOP_TEE: return TILE_OFFSET(1, 2);
		case MINEFIELD_HORIZONTAL_TOP: return TILE_OFFSET(2, 2);
		case MINEFIELD_HORIZONTAL_MIDDLE: return TILE_OFFSET(2, 3);
		case MINEFIELD_HORIZONTAL_BOTTOM: return TILE_OFFSET(2, 5);
		case MINEFIELD_CORNER_TOP_RIGHT: return TILE_OFFSET(4, 2);

		case MINEFIELD_LEFT_TEE: return TILE_OFFSET(0, 3);
		case MINEFIELD_CROSS: return TILE_OFFSET(1, 3);
		case MINEFIELD_RIGHT_TEE: return TILE_OFFSET(4, 3);

		case MINEFIELD_VERTICAL_LEFT: return TILE_OFFSET(0, 4);
		case MINEFIELD_VERTICAL_MIDDLE: return TILE_OFFSET(2, 3);
		case MINEFIELD_VERTICAL_RIGHT: return TILE_OFFSET(4, 4);
		case CLOSED_CELL: return TILE_OFFSET(2, 4);

		case MINEFIELD_CORNER_BOTTOM_LEFT: return TILE_OFFSET(0, 5);
		case MINEFIELD_BOTTOM_TEE: return TILE_OFFSET(1, 5);
		case MINEFIELD_CORNER_BOTTOM_RIGHT: return TILE_OFFSET(4, 5);

		case CORNER_TOP_LEFT: return TILE_OFFSET(7, 2);
		case CORNER_TOP_RIGHT: return TILE_OFFSET(10, 2);
		case CORNER_BOTTOM_LEFT: return TILE_OFFSET(7, 5);
		case CORNER_BOTTOM_RIGHT: return TILE_OFFSET(10, 5);

		case TOP_BORDER__LEFT: return TILE_OFFSET(8, 2);
		case TOP_BORDER__RIGHT: return TILE_OFFSET(9, 2);
		case BOTTOM_BORDER__LEFT: return TILE_OFFSET(8, 5);
		case BOTTOM_BORDER__RIGHT: return TILE_OFFSET(9, 5);

		case LEFT_BORDER__TOP: return TILE_OFFSET(7, 3);
		case LEFT_BORDER__BOTTOM: return TILE_OFFSET(7, 4);
		case RIGHT_BORDER__TOP: return TILE_OFFSET(10, 3);
		case RIGHT_BORDER__BOTTOM: return TILE_OFFSET(10, 4);

		default:
			return TILE_OFFSET(4, 1); // GROUND
	}
}

/* set_tile emulates tile behaviour, but is actually a bitmap copy */
void set_tile(uint8_t dst_x, uint8_t dst_y, uint8_t tile)
{
	uint16_t offs = get_tile_offset(tile);
	for (uint8_t y = 0; y < 8; y++){ 
		for (uint8_t x = 0; x < 8; x++){
			plot_pixel_slow(8*dst_x + x,
			                8*dst_y + y, mines_xpm[16 + 8*(offs >> 8) + y][8*(offs & 0xFF) + x]);
		}
	}
}

void highlight_cell(minefield* mf, int x, int y)
{
    if (mf->state == GAME_OVER)
        set_tile(x, y, EXPLOSION);
    else
        set_tile(x, y, CURSOR);
}

void platform_init()
{
	// Init random number generator:
	srand(time(NULL));

    video_init();
    draw_scenario();
}

void idle_loop(minefield* mf)
{
}

void platform_shutdown()
{
	set_mode(VGA_TEXT_MODE);
    exit(0);
}
