/* kn5000.h: Hardware definitions for Technics KN5000 music keyboard
 *
 * Target: TMP94C241F (TLCS-900/H2, 32-bit CISC, 16 MHz)
 * Display: 320x240 LCD, 8bpp indexed color
 * VGA controller: MN89304 LCD controller with VGA-compatible registers
 */

#ifndef KN5000_H
#define KN5000_H

#include <stdint.h>

/* ========================================================================
 * Memory Map
 * ======================================================================== */

/* Video RAM - 320x240 8bpp framebuffer */
#define VRAM_BASE           ((volatile uint8_t *)0x1A0000)
#define VRAM_SIZE           (320 * 240)  /* 76800 bytes */

/* VGA I/O registers - memory-mapped at 0x170000 + port_number */
#define VGA_IO_BASE         0x170000

/* Extension ROM address range */
#define EXTENSION_ROM_BASE  0x280000
#define EXTENSION_ROM_END   0x2FFFFF
#define EXTENSION_ROM_SIZE  0x80000  /* 512KB */

/* Extension DRAM (shared) */
#define EXTENSION_RAM_BASE  0x200000

/* ========================================================================
 * VGA DAC Registers (palette control)
 * ======================================================================== */

/* VGA DAC write index: write palette index here, then 3 bytes R,G,B to DATA */
#define VGA_DAC_WRITE_INDEX ((volatile uint8_t *)(VGA_IO_BASE + 0x3C8))
/* VGA DAC data: write R, G, B sequentially (6-bit values, 0-63) */
#define VGA_DAC_DATA        ((volatile uint8_t *)(VGA_IO_BASE + 0x3C9))

/* ========================================================================
 * Display Parameters
 * ======================================================================== */

#define LCD_WIDTH           320
#define LCD_HEIGHT          240
#define LCD_BPP             8

/* ========================================================================
 * Game Screen Layout (8x8 tile coordinates)
 * ======================================================================== */

/* Screen dimensions in 8x8 tiles (also defined via -D flags for common code) */
#ifndef SCREEN_WIDTH
#define SCREEN_WIDTH        (LCD_WIDTH / 8)   /* 40 tiles */
#endif
#ifndef SCREEN_HEIGHT
#define SCREEN_HEIGHT       (LCD_HEIGHT / 8)  /* 30 tiles */
#endif

/* Minefield offset: centered on 320x240 screen
 * Minefield is 10x10 cells, each cell = 2x2 tile positions = 20x20 tiles
 * Plus borders: 21x21 tiles
 * Horizontal: (40 - 21) / 2 = ~10 */
#define MINEFIELD_X_OFFSET  10
#define MINEFIELD_Y_OFFSET  4

/* ========================================================================
 * Control Panel Button State (read from firmware RAM)
 *
 * The main firmware polls the control panel via SC1 serial interrupts
 * and stores button bitmaps in RAM. Extension ROMs read these arrays
 * directly — no SC1 access needed.
 *
 * Right panel: 0x8E4A + segment  (segments 0-10, 1 byte each)
 * Left panel:  0x8E5A + segment  (segments 0-10, 1 byte each)
 * ======================================================================== */

/* Segment indices */
#define CPR_SEG4            4
#define CPL_SEG4            4
#define CPL_SEG7            7

/* Right panel segment 4: PART SELECT / CONDUCTOR buttons */
#define CPR_SEG4_UP         0x02  /* bit 1: Part Select RIGHT 2 */
#define CPR_SEG4_LEFT       0x10  /* bit 4: Conductor LEFT */
#define CPR_SEG4_DOWN       0x20  /* bit 5: Conductor RIGHT 2 */
#define CPR_SEG4_RIGHT      0x40  /* bit 6: Conductor RIGHT 1 */

/* Left panel segment 4: VARIATION / MSA buttons */
#define CPL_SEG4_FLAG       0x01  /* bit 0: VARIATION 1 / MSA 1 */
#define CPL_SEG4_OPEN       0x08  /* bit 3: VARIATION 4 */

/* Left panel segment 7 */
#define CPL_SEG7_QUIT       0x08  /* bit 3: EXIT */

/* ========================================================================
 * Tile Graphics
 * ======================================================================== */

#define TILE_WIDTH          8
#define TILE_HEIGHT         8
#define TILE_SIZE           (TILE_WIDTH * TILE_HEIGHT)  /* 64 bytes per tile */

/* ========================================================================
 * External symbols defined in assembly wrapper (main.asm)
 * ======================================================================== */

/* System tick counter (32-bit, incremented at 12,500 Hz by ISR) */
extern volatile uint32_t SYSTEM_TICKS;

/* Game active flag (set by assembly when DISK MENU handler activates game) */
extern volatile uint8_t GAME_ACTIVE;

/* Pointer to tile data in ROM */
extern const uint8_t TILES_DATA[];

/* Pointer to palette data in ROM */
extern const uint8_t PALETTE_DATA[];

/* Number of palette entries */
#define PALETTE_ENTRIES     256

#endif /* KN5000_H */
