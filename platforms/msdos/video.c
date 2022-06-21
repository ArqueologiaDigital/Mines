#include "video-tiles.h"
#include <conio.h>
#include <dos.h>
#include <libi86/string.h>
#include <mines.xpm>
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
    memset(&regs, 0, sizeof(union REGPACK));
    regs.h.ah = SET_MODE;
    regs.h.al = mode;
    intr(VIDEO_INT, &regs);
}

uint8_t hex2int(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;
    return 0xFF; // ERROR!
}

void set_palette()
{
    uint8_t index, r, g, b;
    for (uint8_t i = 1; i <= 15; i++) {
        index = mines_xpm[i][0];
        r = (hex2int(mines_xpm[i][5]) << 4) | hex2int(mines_xpm[i][6]);
        g = (hex2int(mines_xpm[i][7]) << 4) | hex2int(mines_xpm[i][8]);
        b = (hex2int(mines_xpm[i][9]) << 4) | hex2int(mines_xpm[i][10]);

        r = (uint8_t)((double)r) * 7 / 0xff;
        g = (uint8_t)((double)g) * 7 / 0xff;
        b = (uint8_t)((double)b) * 7 / 0xff;

        outp(0x3c8, index);
        outp(0x3c9, r << 3);
        outp(0x3c9, g << 3);
        outp(0x3c9, b << 3);
    }
}

void video_init()
{
    set_mode(VGA_256_COLOR_MODE);
    set_palette();
}

#define TILE_OFFSET(x, y) (((y) << 4) | (x))
static inline uint8_t get_tile_offset(uint8_t tile)
{
    static const uint8_t tile2offset[MAX_VIDEO_TILES] = {
        [ONE_BOMB] = TILE_OFFSET(0, 0),
        [TWO_BOMBS] = TILE_OFFSET(1, 0),
        [THREE_BOMBS] = TILE_OFFSET(2, 0),
        [FOUR_BOMBS] = TILE_OFFSET(3, 0),
        [FIVE_BOMBS] = TILE_OFFSET(4, 0),
        [SIX_BOMBS] = TILE_OFFSET(5, 0),
        [SEVEN_BOMBS] = TILE_OFFSET(6, 0),
        [EIGHT_BOMBS] = TILE_OFFSET(7, 0),
        [BLANK] = TILE_OFFSET(8, 0),
        [CURSOR] = TILE_OFFSET(9, 0),

        [BOMB] = TILE_OFFSET(0, 1),
        [FLAG] = TILE_OFFSET(1, 1),
        [QUESTION_MARK] = TILE_OFFSET(2, 1),
        [EXPLOSION] = TILE_OFFSET(3, 1),

        [MINEFIELD_CORNER_TOP_LEFT] = TILE_OFFSET(0, 2),
        [MINEFIELD_TOP_TEE] = TILE_OFFSET(1, 2),
        [MINEFIELD_HORIZONTAL_TOP] = TILE_OFFSET(2, 2),
        [MINEFIELD_HORIZONTAL_MIDDLE] = TILE_OFFSET(2, 3),
        [MINEFIELD_HORIZONTAL_BOTTOM] = TILE_OFFSET(2, 5),
        [MINEFIELD_CORNER_TOP_RIGHT] = TILE_OFFSET(4, 2),

        [MINEFIELD_LEFT_TEE] = TILE_OFFSET(0, 3),
        [MINEFIELD_CROSS] = TILE_OFFSET(1, 3),
        [MINEFIELD_RIGHT_TEE] = TILE_OFFSET(4, 3),

        [MINEFIELD_VERTICAL_LEFT] = TILE_OFFSET(0, 4),
        [MINEFIELD_VERTICAL_MIDDLE] = TILE_OFFSET(2, 3),
        [MINEFIELD_VERTICAL_RIGHT] = TILE_OFFSET(4, 4),
        [CLOSED_CELL] = TILE_OFFSET(2, 4),

        [MINEFIELD_CORNER_BOTTOM_LEFT] = TILE_OFFSET(0, 5),
        [MINEFIELD_BOTTOM_TEE] = TILE_OFFSET(1, 5),
        [MINEFIELD_CORNER_BOTTOM_RIGHT] = TILE_OFFSET(4, 5),

        [CORNER_TOP_LEFT] = TILE_OFFSET(7, 2),
        [CORNER_TOP_RIGHT] = TILE_OFFSET(10, 2),
        [CORNER_BOTTOM_LEFT] = TILE_OFFSET(7, 5),
        [CORNER_BOTTOM_RIGHT] = TILE_OFFSET(10, 5),

        [TOP_BORDER__LEFT] = TILE_OFFSET(8, 2),
        [TOP_BORDER__RIGHT] = TILE_OFFSET(9, 2),
        [BOTTOM_BORDER__LEFT] = TILE_OFFSET(8, 5),
        [BOTTOM_BORDER__RIGHT] = TILE_OFFSET(9, 5),

        [LEFT_BORDER__TOP] = TILE_OFFSET(7, 3),
        [LEFT_BORDER__BOTTOM] = TILE_OFFSET(7, 4),
        [RIGHT_BORDER__TOP] = TILE_OFFSET(10, 3),
        [RIGHT_BORDER__BOTTOM] = TILE_OFFSET(10, 4),
    };

    if (tile == GROUND) {
        switch (rand() & 15) {
        case 0:
            return TILE_OFFSET(5, 3);
        case 1:
            return TILE_OFFSET(6, 3);
        case 2:
            return TILE_OFFSET(5, 4);
        case 3:
            return TILE_OFFSET(6, 4);
        default:
            return TILE_OFFSET(4, 1);
        }
    }

    uint8_t off = tile2offset[tile];
    return off ? off : tile2offset[GROUND];
}

/* set_tile emulates tile behaviour, but is actually a bitmap copy */
static inline void set_tile_full(uint8_t dst_x, uint8_t dst_y, uint8_t tile, int8_t mask)
{
    static uint8_t __far *video_seg = MK_FP(0xa000, 0);
    uint8_t __far *video_seg_start =
        &video_seg[320 * (int)dst_y * 8 + (int)dst_x * 8];
    const uint16_t offs = get_tile_offset(tile);
    const uint16_t off_high = 16 + 8 * (offs >> 8);
    const uint16_t off_low = 8 * (offs & 0xff);

    if (mask < 0) {
        for (uint8_t y = 0; y < 8; y++) {
            _fmemcpy(video_seg_start, &mines_xpm[off_high + y][off_low], 8);
            video_seg_start += 320;
        }
    } else {
        for (uint8_t y = 0; y < 8; y++) {
            const char *data = &mines_xpm[off_high + y][off_low];
            for (uint8_t x = 0; x < 8; x++) {
                if (data[x] != mask)
                    video_seg_start[x] = data[x];
            }
            video_seg_start += 320;
        }
    }
}

void set_tile(uint8_t dst_x, uint8_t dst_y, uint8_t tile)
{
    return set_tile_full(dst_x, dst_y, tile, -1);
}

void highlight_cell(minefield *mf, int x, int y)
{
    if (mf->state == GAME_OVER)
        set_tile(x, y, EXPLOSION);
    else
        set_tile_full(x, y, CURSOR, '+');
}

void platform_init()
{
    // Init random number generator:
    srand(time(NULL));

    video_init();
    draw_scenario();
}

void idle_loop(minefield *mf) {}

void platform_shutdown()
{
    set_mode(VGA_TEXT_MODE);
    exit(0);
}
