; =============================================================================
; startup.s - Mines for KN5000 (HDAE5000 Extension ROM)
; =============================================================================
; GAS-syntax startup assembly.
; Assembled with: clang -target tlcs900 -c startup.s
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
.equ HANDLER_SLOT,     0x20000C
.equ PARAM_BLOCK,      0x200040
.equ SAVED_SP,         0x200044
.equ DBG_PROGRESS,     0x200050
.equ DBG_FRAME_COUNT,  0x200054
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
; Registers handler 0x016A via RegisterObjectTable, then creates a DISK MENU
; entry and links it to our handler. Follows the original HDAE5000 pattern:
;   1. Register handler via workspace[0x0E0A][0x00E4]
;   2. Create DISK MENU slot via workspace[0x0E0A][0x02C4]
;   3. Set slot+0x00 to link the entry to our handler
; =============================================================================
HANDLER_REGISTRATION:
	push	xix
	push	xiz

	; Debug marker E0 = entering HANDLER_REGISTRATION
	push	xwa
	push	xhl
	ld	xhl, 0x150000
	ld	xwa, 0x00E000FE
	ld	(xhl), xwa
	pop	xhl
	pop	xwa

	; === Step 1: Register handler 0x016A via RegisterObjectTable ===

	; Get table A pointer
	ld	xiz, (WORKSPACE_PTR)
	add	xiz, 0x0E0A
	ld	xiz, (xiz)

	; Build 14-byte param block in RAM at PARAM_BLOCK (0x200040)
	; +0x00: port address (4 bytes) = 0x01600004
	ld	xhl, PARAM_BLOCK
	ld	xwa, 0x01600004
	ld	(xhl), xwa

	; +0x04: handler function (4 bytes) = table_A[0x0168]
	add	xhl, 4
	push	xiz
	add	xiz, 0x0168
	ld	xwa, (xiz)
	pop	xiz
	ld	(xhl), xwa

	; +0x08: record count (2 bytes) = 1 (one sub-object)
	add	xhl, 4
	ld	xwa, 1
	ld	(xhl), xwa

	; +0x0A: data pointer (4 bytes) = MINES_RECORD_TABLE
	add	xhl, 2
	ld	xwa, MINES_RECORD_TABLE
	ld	(xhl), xwa

	; Get RegisterObjectTable function: table_A[0x00E4]
	push	xiz
	add	xiz, 0x00E4
	ld	xix, (xiz)
	pop	xiz

	; Call RegisterObjectTable: WA = handler_id, XBC = param_block_ptr
	ld	xwa, 0x016A
	ld	xbc, PARAM_BLOCK
	call	(xix)

	; Debug marker E1 = after RegisterObjectTable
	push	xwa
	push	xhl
	ld	xhl, 0x150000
	ld	xwa, 0x00E100FE
	ld	(xhl), xwa
	pop	xhl
	pop	xwa

	; === Step 2: Register DISK MENU entry ===

	; Reload table A (registers may be clobbered by RegisterObjectTable)
	ld	xiz, (WORKSPACE_PTR)
	add	xiz, 0x0E0A
	ld	xiz, (xiz)

	; Get DISK MENU registration: table_A[0x02C4]
	push	xiz
	add	xiz, 0x02C4
	ld	xix, (xiz)
	pop	xiz

	ld	xwa, 0x00600002
	call	(xix)

	; XHL = handler slot (menu item structure)
	; Debug marker E2 = after DISK MENU slot registration (preserve XHL!)
	push	xwa
	push	xde
	ld	xde, 0x150000
	ld	xwa, 0x00E200FE
	ld	(xde), xwa
	pop	xde
	pop	xwa

	; Set display name
	ld	xwa, MENU_NAME
	ld	(xhl + 0x2A), xwa

	; Set icon ID (176 = our custom mine icon)
	ld	xwa, 176
	ld	(xhl + 0x32), xwa

	; Link slot to our handler: handler 0x016A, sub-index 0
	ld	xwa, 0x016A0000
	ld	(xhl), xwa

	; Save handler slot pointer
	ld	(HANDLER_SLOT), xhl

	pop	xiz
	pop	xix
	ret

; =============================================================================
; Mines_Handler - Implementation function for our data record
;
; Called by firmware dispatch when events target our handler 0x016A.
; Entry: XWA = object_id, XBC = request, XDE = parameter
; Intercepts activation events to set GAME_ACTIVE flag:
;   0x01C00008 - Sent by firmware DISK MENU when user selects our entry
;   0x01E0009C - Sent by direct event injection (ApPostEvent)
; All other requests are delegated to the default handler.
; =============================================================================
Mines_Handler:
	push	xix
	push	xiz

	; Debug: log event code to DBG_PROGRESS for Lua monitoring
	push	xhl
	ld	xhl, DBG_PROGRESS
	ld	(xhl), xbc
	pop	xhl

	; Check for DISK MENU selection event (0x01C00008)
	ld	xix, 0x01C00008
	cp	xbc, xix
	jr	z, .Lmh_activate

	; Check for direct event injection (0x01E0009C)
	ld	xix, 0x01E0009C
	cp	xbc, xix
	jr	z, .Lmh_activate

	; Not an activation event — delegate to default handler
	jr	.Lmh_delegate

.Lmh_activate:
	; Activation: set GAME_ACTIVE=1, GAME_INITIALIZED=0
	push	xwa
	push	xhl
	ld	xhl, GAME_ACTIVE
	ld	xwa, 0x00000001
	ld	(xhl), xwa

	; Debug marker A1 = Game Activation
	ld	xhl, 0x150000
	ld	xwa, 0x00A100FE
	ld	(xhl), xwa
	pop	xhl
	pop	xwa

	; Don't delegate to default handler on activation —
	; it would show "FD SAVE/LOAD TEST" and interfere with our game
	jr	.Lmh_done

.Lmh_delegate:
	; Delegate to default handler: workspace[0x0E0A][0x00DC]
	; XWA, XBC, XDE are preserved (original call arguments)
	ld	xiz, (WORKSPACE_PTR)
	add	xiz, 0x0E0A
	ld	xiz, (xiz)
	add	xiz, 0x00DC
	ld	xix, (xiz)
	call	(xix)

.Lmh_done:
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

	; Check request code in low byte of XBC register.
	; WORKAROUND: Use 32-bit AND+CP instead of 8-bit `cp a, ...` to avoid
	; LLVM assembler bug #8 (8-bit register encoding mismatch).
	ld	xwa, xbc
	and	xwa, 0xFF
	cp	xwa, 0xA1
	jr	z, .Lreturn_data_ptr
	cp	xwa, 0xA2
	jr	z, .Lreturn_width
	cp	xwa, 0xA3
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
; Must return to firmware (firmware continues boot sequence after this).
; Only stores workspace pointer and registers handlers — does NOT start the game.
; =============================================================================
Boot_Init:
	push	xwa
	push	xde

	; DEBUG: Write marker to AudioMix to confirm Boot_Init was called
	ld	xde, 0x150000
	ld	xwa, 0x00B100FE		; addr=0xFE, data=0xB1 (Boot_Init marker)
	ld	(xde), xwa

	; Store workspace pointer (passed in XWA by firmware)
	; XWA was saved above before we overwrote it, restore it
	pop	xde
	pop	xwa

	; Store workspace pointer for later use
	ld	(WORKSPACE_PTR), xwa

	; Register DISK MENU entry and handler table B callback
	call	HANDLER_REGISTRATION

	; Do NOT set GAME_ACTIVE here. Boot_Init runs during firmware boot;
	; starting the game now would block the boot sequence.
	; The game starts when the user selects our DISK MENU entry,
	; which triggers Activate_Game via the registered handler.

	ret

; =============================================================================
; Register_Frame_Handler - Register for periodic frame callbacks
; =============================================================================
Register_Frame_Handler:
	push	xiz
	push	xix

	ld	xiz, (WORKSPACE_PTR)
	add	xiz, 0x0E88
	ld	xiz, (xiz)
	add	xiz, 0x0108
	ld	xix, (xiz)
	call	(xix)

	pop	xix
	pop	xiz
	ret

; =============================================================================
; Frame_Handler - Called every frame by main firmware (from LABEL_F1E9D0)
;
; On first call (GAME_ACTIVE=1), calls main() which blocks in its game loop.
; When main() returns (player quit), marks GAME_ACTIVE=0 so subsequent
; frames are no-ops.
;
; All registers must be preserved — the firmware's main loop expects them
; intact after this call returns.
; =============================================================================
Frame_Handler:
	; Save all registers (firmware needs them preserved)
	push	xwa
	push	xbc
	push	xde
	push	xhl
	push	xix
	push	xiy
	push	xiz

	; Debug marker F1 to AudioMix (for logic analyzer / MAME oslog)
	ld	xde, 0x150000
	ld	xwa, 0x00F100FE
	ld	(xde), xwa

	; Check GAME_ACTIVE (byte at 0x200000)
	; WORKAROUND: Use 32-bit AND+CP instead of `cp a, 0` to avoid
	; LLVM assembler bug #8 (8-bit register encoding mismatch).
	; `cp a, 0` encodes register W instead of A, checking the wrong byte.
	ld	xhl, GAME_ACTIVE
	ld	xwa, (xhl)
	and	xwa, 0xFF
	cp	xwa, 0
	jr	z, .Lframe_done

	; Game is active — check if C runtime needs initialization
	ld	xhl, GAME_INITIALIZED
	ld	xwa, (xhl)
	and	xwa, 0xFF
	cp	xwa, 0
	jr	nz, .Lskip_init

	; First activation: initialize C runtime
	call	Copy_C_Data
	call	Clear_C_BSS

	; Set GAME_INITIALIZED=1 (keep GAME_ACTIVE=1)
	ld	xhl, GAME_ACTIVE
	ld	xwa, 0x00000101
	ld	(xhl), xwa

.Lskip_init:
	; Save firmware stack pointer, switch to game stack
	ld	(SAVED_SP), xsp
	ld	xsp, STACK_TOP

	; Debug marker: about to call main
	ld	xde, 0x150000
	ld	xwa, 0x00D000FE
	ld	(xde), xwa

	; Call C main() (blocks in game loop until player quits)
	call	main

	; Restore firmware stack pointer
	ld	xsp, (SAVED_SP)

	; main() returned — mark game inactive, clear initialized flag
	ld	xhl, GAME_ACTIVE
	ld	xwa, 0x00000000
	ld	(xhl), xwa

.Lframe_done:
	; Restore all registers
	pop	xiz
	pop	xiy
	pop	xix
	pop	xhl
	pop	xde
	pop	xbc
	pop	xwa
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

; DISK MENU display name (null-terminated string, shown in menu)
MENU_NAME:
	.asciz	"Mines Game"

; Data record table for handler 0x016A (1 record, 24 bytes)
; Referenced by RegisterObjectTable param block; must stay in ROM.
; Record 0 = our DISK MENU entry (linked to slot via 0x016A0000).
MINES_RECORD_TABLE:
	.long	Mines_Handler		; +0x00: implementation function
	.long	0x0160001D		; +0x04: next handler ID (chain to Root module)
	.byte	0x00, 0x00		; +0x08: config size (16-bit)
	.byte	0x00, 0x00		; +0x0A: config flags (16-bit)
	.long	MENU_NAME		; +0x0C: ROM data pointer 1 (component name)
	.long	0			; +0x10: ROM data pointer 2
	.long	0			; +0x14: RAM workspace pointer
