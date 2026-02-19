; =============================================================================
; startup.s - Mines for KN5000 (HDAE5000 Extension ROM)
; =============================================================================
; GAS-syntax startup assembly (replaces main.asm / ASL format).
; Assembled with: clang -target tlcs900 -c startup.s
;
; NOTE: Register+displacement loads are not yet supported by the LLVM
; TLCS-900 assembler. These use an add+load workaround. Stores with
; displacement work fine.
;
; Target: TMP94C241F (TLCS-900/H2) @ 16 MHz
; Display: 320x240 LCD, 8bpp indexed color
; =============================================================================

; =============================================================================
; Hardware Constants
; =============================================================================
.equ VIDEO_RAM_BASE, 0x1A0000
.equ VRAM_SIZE,      76800

; =============================================================================
; RAM Variables (fixed addresses in extension DRAM at 0x200000)
; =============================================================================
.equ GAME_ACTIVE,      0x200000
.equ GAME_INITIALIZED, 0x200001
.equ SYSTEM_TICKS,     0x200004
.equ WORKSPACE_PTR,    0x200008
.equ STACK_TOP,        0x203000

; =============================================================================
; Exports (symbols referenced by C code in all_c.o)
; =============================================================================
.globl GAME_ACTIVE
.globl SYSTEM_TICKS
.globl TILES_DATA
.globl PALETTE_DATA
.globl __udivsi3
.globl __umodsi3

; =============================================================================
; Code Section (.startup, placed at 0x280000 by linker script)
; =============================================================================
.section .startup, "ax", @progbits

; --- XAPR Header (8 bytes at offset 0x00) ---
; Bytes 0-3: "XAPR" magic (checked by main firmware)
; Bytes 4-7: version/flags metadata (not a pointer)
	.ascii	"XAPR"
	.byte	0x34, 0xA1, 0x2F, 0x00

; --- Entry Point 1: Boot_Init (offset 0x08, 8-byte slot) ---
	jp	Boot_Init
	ret
	.byte	0x00, 0x00, 0x00

; --- Entry Point 2: Frame_Handler (offset 0x10, 8-byte slot) ---
	jp	Frame_Handler
	ret
	.byte	0x00, 0x00, 0x00

; --- Entry Point 3: Unused (offset 0x18, 8-byte slot) ---
	ret
	.byte	0x00, 0x00, 0x00

; --- Entry Point 4: Unused (offset 0x1C, 4-byte slot) ---
	ret
	.byte	0x00, 0x00, 0x00

; =============================================================================
; HANDLER_REGISTRATION
;
; Registers our callbacks with the main firmware's dispatch system.
; Called from Boot_Init after workspace pointer is stored.
; Adapted from the real HDAE5000 handler registration protocol.
; =============================================================================
HANDLER_REGISTRATION:
	push	xix
	push	xiz

	; Get workspace pointer
	ld	xiz, (WORKSPACE_PTR)

	; Get handler table A: xiz = *(workspace + 0x0E0A)
	; (reg+disp load not supported, use add+load workaround)
	add	xiz, 0x0E0A
	ld	xiz, (xiz)

	; Get registration function: xix = *(handler_table_A + 0x02C4)
	push	xiz
	add	xiz, 0x02C4
	ld	xix, (xiz)
	pop	xiz

	; Call registration function with ID parameter
	ld	xwa, 0x00600002
	call	(xix)

	; XHL now points to our registered slot
	; Store handler flags
	ld	xwa, 0x016A0005
	ld	(xhl), xwa

	; Store Alloc_Memory_Icon address at slot + 0x2A
	ld	xwa, Alloc_Memory_Icon
	ld	(xhl + 0x2A), xwa

	; Register with handler table B for frame callbacks
	ld	xiz, (WORKSPACE_PTR)

	; Get handler table B: xiz = *(workspace + 0x0E88)
	add	xiz, 0x0E88
	ld	xiz, (xiz)

	; Get handler_B function: xhl = *(handler_table_B + 0x0108)
	add	xiz, 0x0108
	ld	xhl, (xiz)
	call	(xhl)

	pop	xiz
	pop	xix
	ret

; =============================================================================
; Alloc_Memory_Icon - Provides game icon for DISK MENU display
;
; Called by main firmware. Input: XBC = request code (low byte matters)
; Returns: XWA = response (pointer, width, or height)
; =============================================================================
Alloc_Memory_Icon:
	push	xix

	; Check request code in low byte of C register
	ld	a, c
	cp	a, 0xA1
	jr	z, .Lreturn_data_ptr
	cp	a, 0xA2
	jr	z, .Lreturn_width
	cp	a, 0xA3
	jr	z, .Lreturn_height
	jr	.Licon_done

.Lreturn_data_ptr:
	ld	xwa, ICON_DATA
	jr	.Licon_done
.Lreturn_width:
	ld	xwa, 27
	jr	.Licon_done
.Lreturn_height:
	ld	xwa, 27

.Licon_done:
	pop	xix
	ret

; =============================================================================
; Dummy_Return - Unused entry point
; =============================================================================
Dummy_Return:
	ret

; =============================================================================
; Boot_Init - Called once at startup when main firmware detects HDAE5000
;
; Entry: XWA = workspace pointer from main firmware
; =============================================================================
Boot_Init:
	push	xiz
	push	xix
	push	xbc

	; Store workspace pointer (XWA from firmware)
	ld	(WORKSPACE_PTR), xwa

	; Initialize game state
	ld	(GAME_ACTIVE), 0
	ld	(GAME_INITIALIZED), 1

	; Zero system ticks counter (32-bit)
	ld	xwa, 0
	ld	(SYSTEM_TICKS), xwa

	; Register handlers with the main firmware
	call	HANDLER_REGISTRATION
	call	Register_Frame_Handler

	pop	xbc
	pop	xix
	pop	xiz
	ret

; =============================================================================
; Register_Frame_Handler - Register for periodic frame callbacks
; =============================================================================
Register_Frame_Handler:
	push	xiz
	push	xix

	ld	xiz, (WORKSPACE_PTR)
	; (reg+disp load not supported, use add+load workaround)
	add	xiz, 0x0E88
	ld	xiz, (xiz)
	add	xiz, 0x0108
	ld	xix, (xiz)
	call	(xix)

	pop	xix
	pop	xiz
	ret

; =============================================================================
; Frame_Handler - Called every frame by main firmware
; =============================================================================
Frame_Handler:
	; Check: is game initialized? (byte at GAME_INITIALIZED)
	ld	xwa, (GAME_INITIALIZED)
	cp	a, 0
	jr	z, .Lframe_exit

	; Check: is game active? (byte at GAME_ACTIVE)
	ld	xwa, (GAME_ACTIVE)
	cp	a, 0
	jr	z, .Lframe_exit

	; Game is active: call C game loop
	call	C_Game_Frame

.Lframe_exit:
	ret

; =============================================================================
; Activate_Game - Called when DISK MENU selects our entry
;
; Sets up the C runtime and starts the game. Palette loading is handled
; by platform_init() in the C code.
; =============================================================================
Activate_Game:
	; Save firmware registers
	push	xwa
	push	xbc
	push	xde

	; Set game active
	ld	(GAME_ACTIVE), 1

	; Set up game stack
	ld	xwa, STACK_TOP
	ld	xsp, xwa

	; Initialize C runtime
	call	Copy_C_Data
	call	Clear_C_BSS

	; Call C main()
	call	main

	; Game has returned - mark inactive
	ld	(GAME_ACTIVE), 0

	; NOTE: Stack context from firmware is lost after stack switch.
	; This function is currently unreachable (game activation mechanism
	; is not yet wired up). When implemented, the firmware stack pointer
	; must be saved and restored around the stack switch.
	ret

; =============================================================================
; C_Game_Frame - Thin wrapper for frame-callback architecture (currently noop)
; =============================================================================
C_Game_Frame:
	ret

; =============================================================================
; Copy_C_Data - Copy initialized data from ROM (LMA) to RAM (VMA)
;
; Uses 4-byte copies. Linker script must ALIGN .data to 4 bytes.
; Symbols: __data_load (ROM), __data_start (RAM), __data_size (bytes)
; =============================================================================
Copy_C_Data:
	push	xwa
	push	xbc
	push	xde
	push	xhl

	ld	xde, __data_load
	ld	xhl, __data_start
	ld	xbc, __data_size
	cp	xbc, 0
	jr	z, .Lcopy_done
.Lcopy_loop:
	ld	xwa, (xde)
	ld	(xhl), xwa
	add	xde, 4
	add	xhl, 4
	sub	xbc, 4
	jr	nz, .Lcopy_loop
.Lcopy_done:
	pop	xhl
	pop	xde
	pop	xbc
	pop	xwa
	ret

; =============================================================================
; Clear_C_BSS - Zero the C BSS region
;
; Uses 4-byte stores. Linker script must ALIGN .bss to 4 bytes.
; Symbols: __bss_start, __bss_size (bytes)
; =============================================================================
Clear_C_BSS:
	push	xwa
	push	xbc
	push	xde

	ld	xwa, __bss_start
	ld	xbc, __bss_size
	ld	xde, 0
	cp	xbc, 0
	jr	z, .Lbss_done
.Lbss_loop:
	ld	(xwa), xde
	add	xwa, 4
	sub	xbc, 4
	jr	nz, .Lbss_loop
.Lbss_done:
	pop	xde
	pop	xbc
	pop	xwa
	ret

; =============================================================================
; Compiler Runtime Helpers
; =============================================================================

; __udivsi3: unsigned 32-bit division
; Args: XDE = dividend, XBC = divisor
; Returns: XDE = quotient
__udivsi3:
	push	xwa
	push	xhl
	ld	xwa, xde
	ld	xhl, 0
.Ludiv_loop:
	cp	xwa, xbc
	jr	c, .Ludiv_done
	sub	xwa, xbc
	inc	1, xhl
	jr	.Ludiv_loop
.Ludiv_done:
	ld	xde, xhl
	pop	xhl
	pop	xwa
	ret

; __umodsi3: unsigned 32-bit modulo
; Args: XDE = dividend, XBC = divisor
; Returns: XDE = remainder
__umodsi3:
	push	xwa
	ld	xwa, xde
.Lumod_loop:
	cp	xwa, xbc
	jr	c, .Lumod_done
	sub	xwa, xbc
	jr	.Lumod_loop
.Lumod_done:
	ld	xde, xwa
	pop	xwa
	ret

; =============================================================================
; Read-Only Data Section
; =============================================================================
.section .rodata, "a", @progbits

; Tile graphics data (generated by tools/generate_tiles.py)
TILES_DATA:
	.incbin	"build/tiles.bin"

; Palette data (generated by tools/generate_tiles.py)
PALETTE_DATA:
	.incbin	"build/palette.bin"

; Icon data (27x27 pixels, displayed in DISK MENU)
ICON_DATA:
	.incbin	"build/icon.bin"
