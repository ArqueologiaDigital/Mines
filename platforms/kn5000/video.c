/* video.c: Display driver for KN5000
 *
 * Implements tile-based rendering to the 320x240 8bpp LCD framebuffer.
 * VGA registers are memory-mapped at 0x170000 + port_number.
 * VRAM framebuffer is at 0x1A0000.
 */

#include "tiles.h"
#include "codes.h"
#include "kn5000.h"

/* Forward declarations for functions provided by extras.c */
extern int rand(void);
extern void srand(unsigned int v);

/* ========================================================================
 * VGA Palette
 * ======================================================================== */

static void load_palette(void)
{
    const uint8_t *pal = PALETTE_DATA;

    for (uint16_t i = 0; i < PALETTE_ENTRIES; i++) {
        *VGA_DAC_WRITE_INDEX = (uint8_t)i;
        /* Palette data is stored as R, G, B bytes (6-bit VGA values) */
        *VGA_DAC_DATA = *pal++;
        *VGA_DAC_DATA = *pal++;
        *VGA_DAC_DATA = *pal++;
    }
}

/* ========================================================================
 * Tile offset lookup
 *
 * Maps tile codes to sequential offsets into the tile data array.
 * Each tile is 64 bytes (8x8 pixels, 1 byte per pixel).
 * ======================================================================== */

static const uint8_t *get_tile_data(uint8_t tile)
{
    /* GROUND tile has random variations */
    if (tile == GROUND) {
        uint8_t variant = rand() & 3;
        tile = MAX_VIDEO_TILES + variant;
    }

    /* Compute offset = tile * 64 using shift (tile << 6).
     * Avoids general multiply which the TLCS900 backend struggles with. */
    uint16_t offset = (uint16_t)tile << 6;
    return &TILES_DATA[offset];
}

/* ========================================================================
 * Tile rendering
 * ======================================================================== */

/* Compute VRAM offset for tile at (tx, ty) in tile coordinates.
 * Avoids 32-bit multiply which the TLCS900 LLVM backend can't handle yet.
 * offset = ty * 8 * 320 + tx * 8 = ty * 2560 + tx * 8 */
static uint32_t tile_vram_offset(uint8_t tx, uint8_t ty)
{
    /* Use 16-bit additions to build the offset */
    uint32_t offset = 0;
    uint16_t row_stride = LCD_WIDTH * TILE_HEIGHT;  /* 2560 */
    for (uint8_t i = 0; i < ty; i++)
        offset += row_stride;
    offset += (uint16_t)tx * TILE_WIDTH;
    return offset;
}

/* Write a single byte to VRAM. Separate function to avoid volatile
 * pointer arithmetic issues in the TLCS900 LLVM backend. */
static void vram_write(uint32_t offset, uint8_t value)
{
    VRAM_BASE[offset] = value;
}

/* Write an 8x8 tile to the framebuffer.
 * If mask >= 0, pixels matching mask are treated as transparent (skipped). */
static void set_tile_full(uint8_t dst_x, uint8_t dst_y, uint8_t tile, int8_t mask)
{
    uint32_t base_offset = tile_vram_offset(dst_x, dst_y);
    const uint8_t *data = get_tile_data(tile);
    uint32_t row_offset = base_offset;

    for (uint8_t y = 0; y < TILE_HEIGHT; y++) {
        for (uint8_t x = 0; x < TILE_WIDTH; x++) {
            if (mask < 0 || data[x] != (uint8_t)mask)
                vram_write(row_offset + x, data[x]);
        }
        row_offset += LCD_WIDTH;
        data += TILE_WIDTH;
    }
}

void set_tile(uint8_t dst_x, uint8_t dst_y, uint8_t tile)
{
    set_tile_full(dst_x, dst_y, tile, -1);
}

/* ========================================================================
 * Cursor highlighting
 * ======================================================================== */

void highlight_current_cell(minefield *mf)
{
    uint8_t x = CELL_X(mf, mf->current_cell);
    uint8_t target_x = x * 2 + MINEFIELD_X_OFFSET + 1;
    uint8_t y = CELL_Y(mf, mf->current_cell);
    uint8_t target_y = y * 2 + MINEFIELD_Y_OFFSET + 1;
    static uint8_t old_x = 0;
    static uint8_t old_y = 0;

    /* Remove old cursor by redrawing the cell */
    draw_single_cell(mf, old_x, old_y);

    /* Draw cursor with transparency (color 0 = transparent) */
    if (mf->state == PLAYING_GAME)
        set_tile_full(target_x, target_y, CURSOR, 0);

    old_x = x;
    old_y = y;
}

/* ========================================================================
 * Platform interface
 * ======================================================================== */

void platform_init(void)
{
    /* Seed PRNG from system ticks */
    srand((unsigned int)SYSTEM_TICKS);

    /* Load game palette via VGA DAC */
    load_palette();

    /* Clear framebuffer to color 0.
     * VRAM is 76800 bytes = 300 * 256. Use nested loops to avoid
     * 32-bit loop counter which stresses the TLCS900 backend. */
    {
        volatile uint8_t *p = VRAM_BASE;
        for (uint16_t i = 0; i < 300; i++) {
            for (uint16_t j = 0; j < 256; j++) {
                *p++ = 0;
            }
        }
    }
}

void idle_update(minefield *mf)
{
    (void)mf;
    /* No-op: frame timing is managed by the assembly frame handler */
}

void platform_shutdown(void)
{
    /* Restore will be handled by assembly wrapper (palette restore, etc.)
     * Setting GAME_ACTIVE = 0 signals the frame handler to stop calling us */
    GAME_ACTIVE = 0;
}
