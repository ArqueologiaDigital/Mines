#include "tiles.h"
#include "codes.h"
#include <string.h>
#include <time.h>

#define SET_MODE 0x00
#define VGA_TEXT_MODE 0x03
#define VGA_256_COLOR_MODE 0x13

__asm__(".section \".rodata\"\n"
        "mines_bin_start:\n"
        ".incbin \"build/mines.bin\"\n"
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


static inline void outb(uint16_t port, uint8_t value)
{
    asm __volatile("outb %0, %1" : : "Ral"(value), "Nd"(port));
}


static void set_palette(int color, uint8_t r, uint8_t g, uint8_t b)
{
    outb(0x3c8, color);
    outb(0x3c9, r);
    outb(0x3c9, g);
    outb(0x3c9, b);
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


static inline void set_mode(unsigned char mode)
{
    asm __volatile("int $16\n" : : "a"((uint16_t)(SET_MODE << 8 | mode)));
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
        [EMPTY_FLAG] = TILE_OFFSET(1, 1),
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
    static uint8_t __far *video_seg = (uint8_t __far *)0xa0000000;
    uint8_t __far *video_seg_start =
        &video_seg[320 * (unsigned int)dst_y * 8 + (unsigned int)dst_x * 8];
    const uint16_t offs = get_tile_offset(tile);
    const uint16_t off_high = 8 * (offs >> 4);
    const uint16_t off_low = 8 * (offs & 0xf);

    if (mask < 0) {
        for (uint8_t y = 0; y < 8; y++) {
            /* FIXME: use rep movsb here instead? */
            const char *data = &mines_xpm[off_high + y][off_low];
            for (uint8_t x = 0; x < 8; x++) {
                video_seg_start[x] = data[x];
            }
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
    set_tile_full(dst_x, dst_y, tile, -1);
}


void highlight_current_cell(minefield *mf)
{
    uint8_t x = CELL_X(mf, mf->current_cell);
    uint8_t target_x = x * 2 + MINEFIELD_X_OFFSET + 1;
    uint8_t y = CELL_Y(mf, mf->current_cell);
    uint8_t target_y = y * 2 + MINEFIELD_Y_OFFSET + 1;
    static uint8_t old_x = 0;
    static uint8_t old_y = 0;

    // remove old cursor position
    draw_single_cell(mf, old_x, old_y);

    if (mf->state == PLAYING_GAME)
        set_tile_full(target_x, target_y, CURSOR, 2);

    old_x = x;
    old_y = y;
}


#ifdef USE_DEBUG_MODE
/* Thanks to OSDev Wiki for the serial initialization sequence! */

#define SERIAL_PORT_ADDR 0x3f8
static inline uint8_t inb(uint16_t port)
{
    uint8_t value;
    asm __volatile("inb %1, %0" : "=Ral"(value) : "Nd"(port));
    return value;
}


static void serial_debug_init(void)
{
    outb(SERIAL_PORT_ADDR + 1, 0x00);
    outb(SERIAL_PORT_ADDR + 3, 0x80);
    outb(SERIAL_PORT_ADDR + 0, 0x03);
    outb(SERIAL_PORT_ADDR + 1, 0x00);
    outb(SERIAL_PORT_ADDR + 3, 0x03);
    outb(SERIAL_PORT_ADDR + 2, 0xC7);
    outb(SERIAL_PORT_ADDR + 4, 0x0B);
    outb(SERIAL_PORT_ADDR + 4, 0x0F);
}


static int serial_can_transmit(void)
{
    return inb(SERIAL_PORT_ADDR + 5) & 0x20;
}


static int serial_has_byte()
{
    return inb(SERIAL_PORT_ADDR + 5) & 1;
}


void debug_msg(char *msg)
{
    while (*msg) {
        while (!serial_can_transmit())
            ;
        outb(SERIAL_PORT_ADDR, *msg++);
    }
}


void debug(char *msg, uint8_t value)
{
    static const char hex[] = "01234567890abcdef";
    char v[3] = {hex[value >> 4], hex[value & 0xf], 0};

    debug_msg(msg);
    debug_msg(v);
}


void debug_break()
{
    debug_msg("\r\nPress any key to resume");

    while (!serial_has_byte())
        ;
    inb(SERIAL_PORT_ADDR);
}


void debug_mode(uint8_t mode)
{
    debug("debug_mode unimplemented: ", mode);
}
#endif


void platform_init()
{
#ifdef USE_DEBUG_MODE
    serial_debug_init();
#endif

    // Init random number generator:
    srand(time(NULL));

    video_init();
}


void idle_update(minefield *mf) {}


void platform_shutdown()
{
    free(mines_xpm[0]);
    free(mines_xpm);
    set_mode(VGA_TEXT_MODE);
    exit(0);
}
