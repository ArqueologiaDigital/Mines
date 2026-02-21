/* video.c: Display driver for KN5000
 *
 * Implements tile-based rendering to the 320x240 8bpp LCD framebuffer.
 * VGA registers are memory-mapped at 0x170000 + port_number.
 * VRAM framebuffer is at 0x1A0000.
 */

#include "tiles.h"
#include "codes.h"
#include "kn5000.h"

/* Debug marker output via AudioMix port.
 * Writes marker byte to addr 0xFE for tracing in MAME log. */
static void debug_marker(uint8_t marker)
{
    volatile uint32_t *port = (volatile uint32_t *)0x150000;
    *port = ((uint32_t)marker << 16) | 0x00FE;
}

/* Forward declarations for functions provided by extras.c */
extern int rand(void);
extern void srand(unsigned int v);

/* ========================================================================
 * VGA Palette
 * ======================================================================== */

static void load_palette(void)
{
    const uint8_t *pal = PALETTE_DATA;
    /* WORKAROUND for bugs #8 and #9:
     * - Use local pointer variables (bug #9: avoids F2 prefix / SWI 2)
     * - Write DAC index only once (bug #8: per-iteration index write
     *   stores wrong register value). VGA DAC auto-increments after
     *   every 3 RGB bytes, so we only need to set starting index.
     * - The RGB data stores work despite bug #8 because the load and
     *   store encoding errors are consistent (both use same wrong
     *   register), so data flows correctly by accident. */
    volatile uint8_t *dac_index = VGA_DAC_WRITE_INDEX;
    volatile uint8_t *dac_data = VGA_DAC_DATA;

    *dac_index = 0;  /* Start at palette entry 0 */

    for (uint16_t i = 0; i < PALETTE_ENTRIES; i++) {
        *dac_data = *pal++;
        *dac_data = *pal++;
        *dac_data = *pal++;
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

/* Compute VRAM pointer for tile at (tx, ty) in tile coordinates.
 * Returns a pointer to the top-left pixel of the tile in VRAM.
 *
 * WORKAROUND: __attribute__((noinline)) prevents the register allocator
 * from swapping tx/ty when this function is inlined into set_tile
 * (LLVM TLCS-900 bug #10: register allocation x/y swap). */
__attribute__((noinline))
static volatile uint8_t *tile_vram_ptr(uint8_t tx, uint8_t ty)
{
    volatile uint8_t *p = VRAM_BASE;
    uint16_t row_stride = LCD_WIDTH * TILE_HEIGHT;  /* 2560 */
    for (uint8_t i = 0; i < ty; i++)
        p += row_stride;
    p += (uint16_t)tx * TILE_WIDTH;
    return p;
}

/* Copy one row of tile data (8 bytes) to VRAM without transparency.
 * Uses 32-bit copies to avoid LLVM TLCS-900 bug #8 (8-bit register
 * encoding mismatch). 32-bit register encoding is correct (XWA=0,
 * XBC=1, etc.), so 32-bit load-store pairs work properly.
 * Tile positions are 8-byte aligned, so 32-bit access is safe. */
__attribute__((noinline))
static void copy_tile_row(volatile uint8_t *dst, const uint8_t *src)
{
    volatile uint32_t *d = (volatile uint32_t *)dst;
    const uint32_t *s = (const uint32_t *)src;
    d[0] = s[0];
    d[1] = s[1];
}

/* Copy one row with transparency mask. Uses exclusively 32-bit operations
 * to avoid LLVM TLCS-900 bug #8 (8-bit register encoding mismatch).
 * Reads existing VRAM content, replaces non-transparent bytes using
 * 32-bit shift/mask operations, writes result back as 32-bit word. */
__attribute__((noinline))
static void copy_tile_row_masked(volatile uint8_t *dst, const uint8_t *src,
                                  uint8_t mask)
{
    volatile uint32_t *d = (volatile uint32_t *)dst;
    const uint32_t *s = (const uint32_t *)src;
    uint32_t m = (uint32_t)mask;
    uint32_t new_val, result, b;

    /* Pixels 0-3 */
    new_val = s[0];
    result = d[0];
    b = new_val & 0xFF;
    if (b != m) result = (result & ~(uint32_t)0xFF) | b;
    b = (new_val >> 8) & 0xFF;
    if (b != m) result = (result & ~(uint32_t)0xFF00) | (b << 8);
    b = (new_val >> 16) & 0xFF;
    if (b != m) result = (result & ~(uint32_t)0xFF0000) | (b << 16);
    b = (new_val >> 24) & 0xFF;
    if (b != m) result = (result & ~(uint32_t)0xFF000000) | (b << 24);
    d[0] = result;

    /* Pixels 4-7 */
    new_val = s[1];
    result = d[1];
    b = new_val & 0xFF;
    if (b != m) result = (result & ~(uint32_t)0xFF) | b;
    b = (new_val >> 8) & 0xFF;
    if (b != m) result = (result & ~(uint32_t)0xFF00) | (b << 8);
    b = (new_val >> 16) & 0xFF;
    if (b != m) result = (result & ~(uint32_t)0xFF0000) | (b << 16);
    b = (new_val >> 24) & 0xFF;
    if (b != m) result = (result & ~(uint32_t)0xFF000000) | (b << 24);
    d[1] = result;
}

/* Write an 8x8 tile to the framebuffer.
 * If mask >= 0, pixels matching mask are treated as transparent (skipped).
 *
 * WORKAROUND for LLVM TLCS-900 bugs:
 *   Bug #8: 8-bit register encoding corrupts byte loads/stores
 *   Bug #9: F3 prefix for large displacements conflicts with SWI 3
 * Uses 32-bit copies and noinline helpers to avoid both issues. */
static void set_tile_full(uint8_t dst_x, uint8_t dst_y, uint8_t tile, int8_t mask)
{
    volatile uint8_t *vram = tile_vram_ptr(dst_x, dst_y);
    const uint8_t *data = get_tile_data(tile);

    for (uint8_t y = 0; y < TILE_HEIGHT; y++) {
        if (mask < 0)
            copy_tile_row(vram, data);
        else
            copy_tile_row_masked(vram, data, (uint8_t)mask);
        vram += LCD_WIDTH;
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
    debug_marker(0xC0);  /* C0 = platform_init entry */

    /* Seed PRNG from system ticks */
    srand((unsigned int)SYSTEM_TICKS);

    debug_marker(0xC1);  /* C1 = after srand */

    /* Load game palette via VGA DAC */
    load_palette();

    debug_marker(0xC2);  /* C2 = after load_palette */

    /* Clear framebuffer to color 0.
     * VRAM is 76800 bytes = 19200 * 4. Uses 32-bit stores to avoid
     * LLVM TLCS-900 bug #8 (8-bit register encoding mismatch).
     * WORKAROUND: Use do-while with uint32_t counter because the
     * TLCS-900 backend generates broken for-loop code with uint16_t
     * counters (loop exits after 1 iteration). */
    {
        volatile uint32_t *p = (volatile uint32_t *)VRAM_BASE;
        uint32_t count = 19200;
        do {
            *p++ = 0;
            count--;
        } while (count != 0);
    }

    debug_marker(0xC3);  /* C3 = after VRAM clear */
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
