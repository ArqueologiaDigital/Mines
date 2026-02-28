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

static uint8_t saved_fw_palette[768];

void save_fw_palette(void)
{
    /* Reading from VGA DAC doesn't work on MN89304. Do nothing. */
}

void restore_fw_palette(void)
{
    /* Load firmware's default palette from ROM (0xEEFAF0) */
    const uint8_t *pal = (const uint8_t *)0xEEFAF0;
    volatile uint8_t *dac_index = VGA_DAC_WRITE_INDEX;
    volatile uint8_t *dac_data = VGA_DAC_DATA;

    *dac_index = 0;
    for (uint16_t i = 0; i < 256; i++) {
        uint8_t r = *pal++;
        uint8_t g = *pal++;
        uint8_t b = *pal++;
        pal++; /* Skip alpha/padding byte */

        /* Firmware stores 8-bit RGB, DAC needs 4-bit, so shift right by 4 */
        *dac_data = r >> 4;
        *dac_data = g >> 4;
        *dac_data = b >> 4;
    }
}

void load_palette(void)
{
    const uint8_t *pal = PALETTE_DATA;

    *VGA_DAC_WRITE_INDEX = 0;

    for (uint16_t i = 0; i < PALETTE_ENTRIES; i++) {
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

    /* tile * 64 = tile << 6 */
    uint16_t offset = (uint16_t)tile << 6;
    return &TILES_DATA[offset];
}

/* ========================================================================
 * Tile rendering
 * ======================================================================== */

/* Compute VRAM offset for tile at (tx, ty) in tile coordinates. */
static uint16_t tile_vram_offset(uint8_t tx, uint8_t ty)
{
    return (uint16_t)ty * (LCD_WIDTH * TILE_HEIGHT) + (uint16_t)tx * TILE_WIDTH;
}

/* Write an 8x8 tile to the framebuffer.
 * If mask >= 0, pixels matching mask are treated as transparent (skipped). */
static void set_tile_full(uint8_t dst_x, uint8_t dst_y, uint8_t tile, int8_t mask)
{
    uint16_t offset = tile_vram_offset(dst_x, dst_y);
    const uint8_t *data = get_tile_data(tile);

    for (uint8_t y = 0; y < TILE_HEIGHT; y++) {
        for (uint8_t x = 0; x < TILE_WIDTH; x++) {
            uint8_t pixel = data[y * TILE_WIDTH + x];
            if (mask < 0 || pixel != (uint8_t)mask)
                VRAM_BASE[offset + x] = pixel;
        }
        offset += LCD_WIDTH;
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

/* Clear VRAM to black (palette index 0). Implemented in assembly. */
extern void clear_vram(void);

void platform_init(void)
{
    /* Seed PRNG from system ticks */
    srand((unsigned int)SYSTEM_TICKS);

    // save_fw_palette();
    // load_palette();

    clear_vram();
}

/* Yield to firmware — implemented in startup.s.
 * Saves game state (registers + stack), returns control to firmware's
 * main loop. Firmware processes SC1 serial data (updating button state
 * at 0x8E4A), then calls Frame_Handler on the next frame, which
 * resumes execution here. */
extern void yield_to_firmware(void);

/* Full minefield redraw — defined in common/8x8_tiles.h, draws borders + cells */
extern void draw_minefield(minefield *mf);

void idle_update(minefield *mf)
{
    /* Clear firmware's control panel event queues to prevent it from
     * performing original actions while the game is running. 
     * Do NOT clear if we are quitting, so the firmware sees the EXIT button
     * and correctly returns to the main menu. */
    if (mf->state != QUIT) {
        *CPANEL_RX_READ_PTR = *CPANEL_RX_WRITE_PTR;
        *CPANEL_EVENT_READ_PTR = *CPANEL_EVENT_WRITE_PTR;
    }

    /* Ensure our palette is loaded and board is redrawn. This counters
     * any firmware UI elements that might bypass the DISP_DISABLE_FLAG
     * or any palette changes made by the firmware. 
     * We don't call clear_vram() here to avoid flickering. */
    // load_palette();
    draw_minefield(mf);

    /* Yield back to firmware until next frame. This lets the firmware's
     * main loop run between game frames, processing SC1 control panel
     * data and updating button state arrays at 0x8E4A/0x8E5A. */
    yield_to_firmware();
}

void platform_shutdown(void)
{
    restore_fw_palette();
    /* Cleanup handled by startup.s assembly wrapper on main() return */
}
