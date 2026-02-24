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
.equ GAME_SAVED_SP,    0x200048
.equ DBG_PROGRESS,     0x200050
.equ DBG_FRAME_COUNT,  0x200054
.equ DBG_HANDLER_XWA,  0x200058
.equ DBG_HANDLER_XDE,  0x20005C
.equ HANDLER_WORKSPACE, 0x200100	; 64 bytes workspace for handler lifecycle
.equ STACK_TOP,        0x203000

; =============================================================================
; Exports (symbols referenced by C code in all_c.o)
; =============================================================================
.globl GAME_ACTIVE
.globl SYSTEM_TICKS
.globl TILES_DATA
.globl PALETTE_DATA
.globl yield_to_firmware
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
; Registers handler 0x016A with 13 sub-object records (matching the original
; HDAE5000 structure), then customizes the firmware's auto-created DISK MENU
; entry with our name and icon.
;
; The firmware detects the XAPR header and creates a default "HARD DISK MAIN
; MENU" entry linked to 0x016A0005 (sub-index 5 = HDTitleMenu position).
; Our data table places Mines_Handler at record[5] so the firmware's entry
; dispatches to our code. Other records are stubs (Dummy_Return).
;
; Steps:
;   1. Register handler 0x016A via RegisterObjectTable (13 records)
;   2. Get firmware's DISK MENU slot via workspace[0x0E0A][0x02C4]
;   3. Customize slot: set name="Mines Game", icon=176, link=0x016A0005
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

	; +0x08: record count (2 bytes) = 13 sub-objects
	; Must match the original HDAE5000 structure. The firmware's
	; DISK MENU entry links to sub-index 5 (0x016A0005), so the
	; data table must have at least 6 records. We provide all 13
	; for compatibility with any firmware code that expects them.
	add	xhl, 4
	ld	xwa, 13
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

	; === Step 2: Customize DISK MENU entry ===
	; The firmware auto-creates a "HARD DISK MAIN MENU" entry when it
	; detects the XAPR header, but that entry links to the original
	; HDAE5000 handler (floppy dialog). We must create our own entry
	; via table_A[0x02C4] and link it to 0x016A0005 (our record[5]).

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
	; Set display name at slot+0x2A
	ld	xwa, MENU_NAME
	ld	(xhl + 0x2A), xwa

	; Set icon ID at slot+0x32 (176 = our custom mine icon)
	ld	xwa, 176
	ld	(xhl + 0x32), xwa

	; Link slot to our handler: 0x016A sub-index 5 (record[5] = Mines_Handler)
	ld	xwa, 0x016A0005
	ld	(xhl), xwa

	; Save handler slot pointer
	ld	(HANDLER_SLOT), xhl

	pop	xiz
	pop	xix
	ret

; =============================================================================
; Mines_Handler - Implementation function for our data record
;
; Called by SendEvent when events target our sub-object (0x016A0000).
; Entry: XWA = object_id, XBC = event code, XDE = parameter
;
; Intercepts activation events to set GAME_ACTIVE flag and SKIPS delegation:
;   0x01C00008 - Button-press activation (DISK MENU user selection)
;   0x01E0009C - Programmatic activation (PostEvent injection)
;
; All other events are delegated to the default lifecycle handler
; (table_A[0x00DC]) which manages DISK MENU rendering, item enumeration,
; focus, and other lifecycle operations. Skipping delegation for these
; events breaks the DISK MENU (only 1 item visible, buttons misbehave).
;
; Activation events MUST skip delegation — the default handler would show
; "FD SAVE/LOAD TEST" dialog, interfering with the game.
; =============================================================================
Mines_Handler:
	push	xix
	push	xiz

	; Debug: log event code and object_id for diagnosis
	push	xhl
	ld	xhl, DBG_PROGRESS
	ld	(xhl), xbc		; 0x200050 = event code (XBC)
	ld	xhl, DBG_HANDLER_XWA
	ld	(xhl), xwa		; 0x200058 = object_id (XWA)
	pop	xhl

	; DIAGNOSTIC: Write marker 0xDD to AudioMix[0xFD] = "Mines_Handler called"
	push	xwa
	push	xhl
	ld	xhl, 0x150000
	ld	xwa, 0x00DD00FD
	ld	(xhl), xwa
	pop	xhl
	pop	xwa

	; Check for DISK MENU selection event (0x01C00008)
	ld	xix, 0x01C00008
	cp	xbc, xix
	jr	z, .Lmh_activate

	; Check for programmatic activation (0x01E0009C)
	ld	xix, 0x01E0009C
	cp	xbc, xix
	jr	z, .Lmh_activate

	; All other events: delegate to default lifecycle handler.
	; Required for DISK MENU rendering and item management.
	jr	.Lmh_delegate

.Lmh_activate:
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
	; Skip delegation — default handler would show "FD SAVE/LOAD TEST"
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

	; Register handler 0x016A and customize the firmware's DISK MENU entry
	call	HANDLER_REGISTRATION

	ret

; =============================================================================
; Frame_Handler - Called every frame by main firmware
;
; Cooperative architecture: game yields to firmware between frames via
; yield_to_firmware(), allowing the firmware's main loop to process SC1
; serial data and update button state arrays at 0x8E4A/0x8E5A.
;
; First frame:  init C runtime, call main() (runs until first yield)
; Next frames:  resume game from where yield_to_firmware() was called
; Game exit:    main() returns normally, mark GAME_ACTIVE=0
;
; All registers must be preserved — the firmware's main loop expects them
; intact after this call returns.
; =============================================================================
Frame_Handler:
	; Save all firmware registers
	push	xwa
	push	xbc
	push	xde
	push	xhl
	push	xix
	push	xiy
	push	xiz

	; Debug marker FF = Frame_Handler entry (every call)
	ld	xhl, 0x150000
	ld	xwa, 0x00FF00FE
	ld	(xhl), xwa

	; Check GAME_ACTIVE (byte at 0x200000)
	; WORKAROUND: Use 32-bit AND+CP instead of `cp a, 0` to avoid
	; LLVM assembler bug #8 (8-bit register encoding mismatch).
	ld	xhl, GAME_ACTIVE
	ld	xwa, (xhl)
	and	xwa, 0xFF
	cp	xwa, 0
	jrl	z, .Lframe_done

	; Debug marker FE = GAME_ACTIVE was nonzero
	ld	xhl, 0x150000
	ld	xwa, 0x00FE00FE
	ld	(xhl), xwa

	; Game is active — check if C runtime needs initialization
	ld	xhl, GAME_INITIALIZED
	ld	xwa, (xhl)
	and	xwa, 0xFF
	cp	xwa, 0
	jr	nz, .Lresume_game

	; === First activation: init C runtime and start game ===
	; Save firmware stack pointer
	ld	xwa, xsp
	ld	(SAVED_SP), xwa

	; Switch to game stack
	ld	xsp, STACK_TOP

	; Set initialized flag (GAME_ACTIVE already 1)
	ld	xhl, GAME_INITIALIZED
	ld	xwa, 1
	ld	(xhl), xwa

	; Initialize C runtime
	call	Copy_C_Data
	call	Clear_C_BSS

	; Debug marker D0 = about to call main()
	ld	xhl, 0x150000
	ld	xwa, 0x00D000FE
	ld	(xhl), xwa

	; Call main() — game runs until first yield_to_firmware()
	; Subsequent frames resume from yield_to_firmware via .Lresume_game
	call	main

	; main() returned — game exited normally
	; Debug marker D9 = main() returned
	ld	xhl, 0x150000
	ld	xwa, 0x00D900FE
	ld	(xhl), xwa

	; Clear game state
	ld	xhl, GAME_ACTIVE
	ld	xwa, 0
	ld	(xhl), xwa
	ld	xhl, GAME_INITIALIZED
	ld	(xhl), xwa

	; Switch back to firmware stack and return
	ld	xwa, (SAVED_SP)
	ld	xsp, xwa
	jr	.Lframe_done

.Lresume_game:
	; Subsequent frames: resume game from yield_to_firmware()
	; Save firmware stack pointer
	ld	xwa, xsp
	ld	(SAVED_SP), xwa

	; Restore game stack pointer
	ld	xwa, (GAME_SAVED_SP)
	ld	xsp, xwa

	; Pop game registers (saved by yield_to_firmware)
	pop	xiz
	pop	xiy
	pop	xix
	pop	xhl
	pop	xde
	pop	xbc
	pop	xwa

	; Return to game code (after call to yield_to_firmware)
	ret

.Lframe_done:
	; Restore all firmware registers
	pop	xiz
	pop	xiy
	pop	xix
	pop	xhl
	pop	xde
	pop	xbc
	pop	xwa
	ret

; =============================================================================
; yield_to_firmware - Cooperative yield from game to firmware
;
; Called from idle_update() at the end of each game loop iteration.
; Saves game registers and stack, switches to firmware stack, and
; returns to firmware via .Lframe_done. The firmware's main loop then
; runs (processing SC1 data, updating button state, etc.) until the
; next frame, when Frame_Handler resumes the game.
; =============================================================================
yield_to_firmware:
	; Save all game registers on game stack
	push	xwa
	push	xbc
	push	xde
	push	xhl
	push	xix
	push	xiy
	push	xiz

	; Save game stack pointer
	ld	xwa, xsp
	ld	(GAME_SAVED_SP), xwa

	; Load firmware SP and switch
	ld	xwa, (SAVED_SP)
	ld	xsp, xwa

	; Return to firmware (same as .Lframe_done)
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

; Data record table for handler 0x016A (13 records, 24 bytes each = 312 bytes)
; Must match the original HDAE5000 structure: the firmware's DISK MENU entry
; links to sub-index 5 (HDTitleMenu). Records 0-4 and 6-12 are stubs that
; delegate to the default handler. Record 5 is our Mines_Handler.
;
; Each record: +0x00 func(4), +0x04 next(4), +0x08 size(2), +0x0A flags(2),
;              +0x0C data1(4), +0x10 data2(4), +0x14 workspace(4) = 24 bytes
MINES_RECORD_TABLE:
; Record 0 (SelectList) - stub
	.long	Dummy_Return, 0xFFFFFFFF
	.byte	0x00, 0x00, 0x00, 0x00
	.long	0, 0, 0
; Record 1 (DbMemoCl) - stub
	.long	Dummy_Return, 0xFFFFFFFF
	.byte	0x00, 0x00, 0x00, 0x00
	.long	0, 0, 0
; Record 2 (TtlScreenR) - stub
	.long	Dummy_Return, 0xFFFFFFFF
	.byte	0x00, 0x00, 0x00, 0x00
	.long	0, 0, 0
; Record 3 (AcHddNamingWindow) - stub
	.long	Dummy_Return, 0xFFFFFFFF
	.byte	0x00, 0x00, 0x00, 0x00
	.long	0, 0, 0
; Record 4 (IvHddNaming) - stub
	.long	Dummy_Return, 0xFFFFFFFF
	.byte	0x00, 0x00, 0x00, 0x00
	.long	0, 0, 0
; Record 5 (HDTitleMenu) - OUR HANDLER (firmware DISK MENU entry targets this)
	.long	Mines_Handler
	.long	0x0160001D		; next: chain to Root module (same as original)
	.byte	0x36, 0x00		; config size = 54 (same as original)
	.byte	0x00, 0x00		; config flags = 0
	.long	MENU_NAME		; ROM data pointer 1
	.long	0			; ROM data pointer 2
	.long	HANDLER_WORKSPACE	; RAM workspace for default handler lifecycle
; Record 6 (TtlScreenR2) - stub
	.long	Dummy_Return, 0xFFFFFFFF
	.byte	0x00, 0x00, 0x00, 0x00
	.long	0, 0, 0
; Record 7 (TtlScreenR3) - stub
	.long	Dummy_Return, 0xFFFFFFFF
	.byte	0x00, 0x00, 0x00, 0x00
	.long	0, 0, 0
; Record 8 (AcWindowPage1) - stub
	.long	Dummy_Return, 0xFFFFFFFF
	.byte	0x00, 0x00, 0x00, 0x00
	.long	0, 0, 0
; Record 9 (IvScreenR2) - stub
	.long	Dummy_Return, 0xFFFFFFFF
	.byte	0x00, 0x00, 0x00, 0x00
	.long	0, 0, 0
; Record 10 (AcLanguageText1) - stub
	.long	Dummy_Return, 0xFFFFFFFF
	.byte	0x00, 0x00, 0x00, 0x00
	.long	0, 0, 0
; Record 11 (LyricBox) - stub
	.long	Dummy_Return, 0xFFFFFFFF
	.byte	0x00, 0x00, 0x00, 0x00
	.long	0, 0, 0
; Record 12 (FDFileSelect) - stub
	.long	Dummy_Return, 0xFFFFFFFF
	.byte	0x00, 0x00, 0x00, 0x00
	.long	0, 0, 0
