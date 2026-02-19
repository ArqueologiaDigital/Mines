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
 * TMP94C241F Special Function Registers (memory-mapped I/O)
 * Used for SC1 serial (control panel) and system timer
 * ======================================================================== */

/* SC1 Serial Channel (Control Panel communication, 250 kHz synchronous) */
#define SC1BUF              (*(volatile uint8_t *)0x230)
#define SC1CR               (*(volatile uint8_t *)0x232)
#define SC1MOD              (*(volatile uint8_t *)0x234)
#define BR1CR               (*(volatile uint8_t *)0x236)

/* Interrupt registers */
#define INTES1              (*(volatile uint8_t *)0x20D)
#define INTCLR              (*(volatile uint8_t *)0x068)

/* SC1 interrupt flag bits */
#define INTRX1_BIT          0x08  /* bit 3: RX complete */
#define INTTX1_CLR          0x23  /* Clear INTTX1 pending */
#define INTRX1_CLR          0x22  /* Clear INTRX1 pending */

/* System timer - INTT1 at 12,500 Hz (80us/tick) */
/* The SYSTEM_TICKS variable is incremented by the ISR in the assembly wrapper */

/* Port E - used for HDAE5000 presence detection */
#define PE_REG              (*(volatile uint8_t *)0x10C)

/* ========================================================================
 * Control Panel Serial Protocol
 *
 * The KN5000 control panel uses SC1 synchronous serial at 250 kHz.
 * Two sub-panels are addressed:
 *   Left panel:  command byte 0x20
 *   Right panel: command byte 0xE0
 *
 * Query protocol: send command, segment, then 2 dummy bytes (0xFF)
 * to clock in header (discarded) and button bitmap response.
 * ======================================================================== */

#define CPANEL_LEFT         0x20
#define CPANEL_RIGHT        0xE0

/* Button mapping (same as Another World project):
 *
 * CPR_SEG4 (right panel, segment 4):
 *   bit 1 = UP       (Part Select: Right 2)
 *   bit 4 = LEFT     (Conductor: Left)
 *   bit 5 = DOWN     (Conductor: Right 2)
 *   bit 6 = RIGHT    (Conductor: Right 1)
 *
 * CPL_SEG4 (left panel, segment 4):
 *   bit 3 = OPEN     (Variation 4)
 *
 * CPL_SEG2 (left panel, segment 2):
 *   bit 7 = FLAG     (Page Up)
 *
 * CPL_SEG7 (left panel, segment 7):
 *   bit 3 = QUIT     (Exit)
 */

/* Right panel segment 4 button bits */
#define CPR_SEG4            4
#define CPR_SEG4_UP         0x02  /* bit 1 */
#define CPR_SEG4_LEFT       0x10  /* bit 4 */
#define CPR_SEG4_DOWN       0x20  /* bit 5 */
#define CPR_SEG4_RIGHT      0x40  /* bit 6 */

/* Left panel segment 4 button bits */
#define CPL_SEG4            4
#define CPL_SEG4_OPEN       0x08  /* bit 3 */

/* Left panel segment 2 button bits */
#define CPL_SEG2            2
#define CPL_SEG2_FLAG       0x80  /* bit 7 */

/* Left panel segment 7 button bits */
#define CPL_SEG7            7
#define CPL_SEG7_QUIT       0x08  /* bit 3 */

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
