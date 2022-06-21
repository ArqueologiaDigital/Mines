#include "video-tiles.h"
#include <conio.h>
#include <dos.h>
#include <libi86/string.h>
#include <string.h>
#include <time.h>

#define SET_MODE 0x00
#define VIDEO_INT 0x10
#define VGA_TEXT_MODE 0x03
#define VGA_256_COLOR_MODE 0x13

__asm__(".section \".rodata\"\n"
        "mines_bin_start:\n"
        ".incbin \"mines.bin\"\n"
        "mines_bin_end:\n"
        ".previous");
extern const char mines_bin_start[];
extern const char mines_bin_end[];

struct img_header {
    uint16_t width;
    uint16_t height;
    uint8_t num_colors;
} __attribute__((packed));

struct img_pal_entry {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} __attribute__((packed));

static char **mines_xpm;

static void set_palette(int color, uint8_t r, uint8_t g, uint8_t b)
{
    outp(0x3c8, color);
    outp(0x3c9, r);
    outp(0x3c9, g);
    outp(0x3c9, b);
}

static char **decode_mines_bin(void)
{
    struct img_header header;
    const char *p;

    memcpy(&header, mines_bin_start, sizeof(struct img_header));
    p = mines_bin_start + sizeof(struct img_header);

    char *outbuf = malloc(header.width * header.height);
    if (!outbuf)
        return NULL;

    for (uint8_t i = 0; i < header.num_colors; i++) {
        struct img_pal_entry pal;

        memcpy(&pal, p, sizeof(struct img_pal_entry));
        p += sizeof(struct img_pal_entry);

        set_palette(i, pal.r, pal.g, pal.b);
    }

    char *outbufptr = outbuf;
    uint8_t buffer;
    uint8_t nibbles_in_buffer = 0;
    uint8_t state = 0;
    uint8_t count = 0;
    while (p < mines_bin_end) {
        if (!nibbles_in_buffer) {
            buffer = *p++;
            nibbles_in_buffer = 2;
        }

        uint8_t nibble = (buffer >> 4) & 0xf;
        buffer <<= 4;
        nibbles_in_buffer--;

        if (state == 0) {
            if (nibble == 0xf) {
                state = 1;
            } else {
                *outbufptr++ = nibble;
            }
        } else if (state == 1) {
            count = nibble + 3;
            state = 2;
        } else {
            for (int i = 0; i < count; i++) {
                *outbufptr++ = nibble;
            }
            state = 0;
        }
    }

    char **out = malloc(header.height * sizeof(char *));
    if (!out) {
        free(outbuf);
        return NULL;
    }

    for (int i = 0; i < header.height; i++) {
        out[i] = outbuf;
        outbuf += header.width;
    }

    return out;
}

static void set_mode(unsigned char mode)
{
    intr(VIDEO_INT, &(union REGPACK){.h = {.ah = SET_MODE, .al = mode}});
}

static void video_init(void)
{
    set_mode(VGA_256_COLOR_MODE);
    mines_xpm = decode_mines_bin();
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
        [MINEFIELD_HORIZONTAL_BOTTOM] = TILE_OFFSET(2, 4),
        [MINEFIELD_CORNER_TOP_RIGHT] = TILE_OFFSET(4, 2),

        [MINEFIELD_LEFT_TEE] = TILE_OFFSET(0, 3),
        [MINEFIELD_CROSS] = TILE_OFFSET(1, 3),
        [MINEFIELD_RIGHT_TEE] = TILE_OFFSET(4, 3),

        [MINEFIELD_VERTICAL_LEFT] = TILE_OFFSET(0, 4),
        [MINEFIELD_VERTICAL_MIDDLE] = TILE_OFFSET(2, 3),
        [MINEFIELD_VERTICAL_RIGHT] = TILE_OFFSET(4, 4),
        [CLOSED_CELL] = TILE_OFFSET(10, 0),

        [MINEFIELD_CORNER_BOTTOM_LEFT] = TILE_OFFSET(5, 4),
        [MINEFIELD_BOTTOM_TEE] = TILE_OFFSET(1, 4),
        [MINEFIELD_CORNER_BOTTOM_RIGHT] = TILE_OFFSET(6, 4),

        [CORNER_TOP_LEFT] = TILE_OFFSET(7, 1),
        [CORNER_TOP_RIGHT] = TILE_OFFSET(10, 1),
        [CORNER_BOTTOM_LEFT] = TILE_OFFSET(7, 4),
        [CORNER_BOTTOM_RIGHT] = TILE_OFFSET(10, 4),

        [TOP_BORDER__LEFT] = TILE_OFFSET(8, 1),
        [TOP_BORDER__RIGHT] = TILE_OFFSET(9, 1),
        [BOTTOM_BORDER__LEFT] = TILE_OFFSET(8, 4),
        [BOTTOM_BORDER__RIGHT] = TILE_OFFSET(9, 4),

        [LEFT_BORDER__TOP] = TILE_OFFSET(7, 2),
        [LEFT_BORDER__BOTTOM] = TILE_OFFSET(7, 3),
        [RIGHT_BORDER__TOP] = TILE_OFFSET(10, 2),
        [RIGHT_BORDER__BOTTOM] = TILE_OFFSET(10, 3),
    };

    if (tile == GROUND) {
        switch (rand() & 15) {
        case 0:
            return TILE_OFFSET(8, 2);
        case 1:
            return TILE_OFFSET(8, 3);
        case 2:
            return TILE_OFFSET(9, 2);
        case 3:
            return TILE_OFFSET(9, 3);
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
    const uint16_t off_high = 8 * (offs >> 4);
    const uint16_t off_low = 8 * (offs & 0xf);

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
        set_tile_full(x, y, CURSOR, 2);
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
    free(mines_xpm[0]);
    free(mines_xpm);
    set_mode(VGA_TEXT_MODE);
    exit(0);
}
