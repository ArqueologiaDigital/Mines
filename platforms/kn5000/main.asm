; =============================================================================
; main.asm - Mines for KN5000 (HDAE5000 Extension ROM)
; =============================================================================
; Assembly wrapper that packages the compiled C game code as a 512KB HDAE5000
; extension ROM. When installed, the game appears in the DISK MENU.
;
; Build: make (produces mines_kn5000.bin, 512KB)
;
; Target: TMP94C241F (TLCS-900/H2) @ 16 MHz
; Display: 320x240 LCD, 8bpp indexed color
;
; ROM Layout (0x280000-0x2FFFFF):
;   0x280000: XAPR header (8 bytes)
;   0x280008: JP Boot_Init
;   0x28000C: JP Frame_Handler
;   0x280010: JP Dummy (unused entry point)
;   0x280020: Handler_Registration (~750 bytes, from real HDAE5000)
;   0x280400: Assembly glue code
;   0x281000: Compiled C code (.text)
;   0x2Exxxx: Read-only data (tiles, palette)
;   0x2FFFFF: ROM end
; =============================================================================

	cpu 96c141		; TLCS-900/H target
	page 0
	maxmode on

; =============================================================================
; Include shared definitions from kn5000-roms-disasm
; =============================================================================
	include "sfr_tmp94c241.asm"
	include "vga_constants.asm"
	include "tmp94c241.inc"
	include "local_macros.inc"

; =============================================================================
; RAM Section (Extension DRAM area at 0x200000)
; =============================================================================
	ORG 0200000h

; --- Game state variables ---
GAME_ACTIVE:		DB ?		; 0 = game not running, 1 = game running
GAME_INITIALIZED:	DB ?		; 0 = first boot, 1 = handlers registered
			DB ?, ?		; padding for alignment

; --- System tick counter (incremented by main firmware's INTT1 ISR) ---
; We read the main firmware's tick counter rather than running our own ISR.
; This is stored separately so C code can reference it.
SYSTEM_TICKS:		DD ?

; --- Workspace pointer from main firmware ---
WORKSPACE_PTR:		DD ?

; --- Saved palette (256 entries * 3 bytes = 768 bytes) ---
SAVED_PALETTE:		DB 768 DUP (?)

; --- C globals (.bss area for compiled C code) ---
	ORG 0201000h
C_BSS_START:
	DB 01000h DUP (?)	; 4KB for C globals (.bss)
C_BSS_END:

; --- Static heap (used by calloc in extras.c) ---
	; Located within C BSS, managed by extras.c's static heap array

; --- Stack ---
	ORG 0203000h
STACK_TOP:			; Stack grows down from here

; =============================================================================
; Extension Board ROM Layout
; =============================================================================
	ORG 0280000h

; --- XAPR Header (8 bytes) ---
EXTENSION_HEADER:
	db 'XAPR'			; Magic signature
	dd HANDLER_REGISTRATION		; Pointer to handler registration code

; --- Entry Point 1: Boot Init (0x280008) ---
	JP Boot_Init

; --- Entry Point 2: Frame Handler (0x28000C) ---
	JP Frame_Handler

; --- Entry Point 3: Unused (0x280010) ---
	JP Dummy_Return

; --- Padding to Handler_Registration at 0x280020 ---
	ORG 0280014h
	db 0Ch DUP (0FFh)

; =============================================================================
; Handler_Registration (0x280020)
;
; This section registers our callbacks with the main firmware's dispatch system.
; Adapted from the real HDAE5000 handler registration protocol.
;
; The main firmware calls through the workspace pointer's handler tables to
; register expansion board callbacks. We register:
;   - Alloc_Memory handlers that provide our icon image data
;   - UI page handlers for the DISK MENU integration
;
; For a homebrew ROM, we provide simplified handlers that register our
; game icon and DISK MENU entry.
; =============================================================================
	ORG 0280020h
HANDLER_REGISTRATION:
	push	XIX
	push	XIZ

	; Get workspace handler table A
	ld	XIZ, (WORKSPACE_PTR)
	ld	XIZ, (XIZ + 0E0Ah)	; WS_OFFSET_HANDLERS_A

	; Register our Alloc_Memory_4 handler (provides icon data)
	; This makes the main firmware display our icon in the DISK MENU
	ld	XIX, (XIZ + 02C4h)	; WS_HANDLER_A_FUNC1 - registration function
	ld	XWA, 00600002h		; Handler registration ID
	call	T, XIX			; Call registration function
	; XHL now points to our registered slot
	ld	XWA, 016A0005h		; Handler flags/ID (same as real HDAE5000)
	ld	(XHL), XWA		; Store flags
	lda	XWA, Alloc_Memory_Icon	; Our icon handler
	ld	(XHL + 02Ah), XWA	; Store handler address at offset 0x2A

	; Register with handler table B for frame callbacks
	ld	XIZ, (WORKSPACE_PTR)
	ld	XIZ, (XIZ + 0E88h)	; WS_OFFSET_HANDLERS_B

	; Get handler B function 1 (for periodic frame callbacks)
	ld	XHL, (XIZ + 0108h)	; WS_HANDLER_B_FUNC1
	call	T, XHL

	pop	XIZ
	pop	XIX
	ret

; =============================================================================
; Alloc_Memory_Icon - Provides game icon for DISK MENU display
;
; Called by main firmware when it needs icon/display parameters.
; Input: XBC = request code
; Returns display parameters based on request:
;   0x01E000A1 → pointer to image data
;   0x01E000A2 → width (27 for icon)
;   0x01E000A3 → height (27 for icon)
; =============================================================================
Alloc_Memory_Icon:
	push	XIX

	; Check request code in low byte of XBC
	ld	A, C
	cp	A, 0A1h
	jr	Z, .return_data_ptr
	cp	A, 0A2h
	jr	Z, .return_width
	cp	A, 0A3h
	jr	Z, .return_height
	jr	.done

.return_data_ptr:
	lda	XWA, ICON_DATA
	jr	.done
.return_width:
	ld	XWA, 27			; Icon width
	jr	.done
.return_height:
	ld	XWA, 27			; Icon height

.done:
	pop	XIX
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
	push	XIZ
	push	XIX
	push	XBC

	; Store workspace pointer
	ld	(WORKSPACE_PTR), XWA

	; Initialize game state
	ld	(GAME_ACTIVE), 0
	ld	(GAME_INITIALIZED), 1

	; Zero the system ticks counter
	ld	XWA, 0
	ld	(SYSTEM_TICKS), XWA

	; Register our handlers with the main firmware
	call	HANDLER_REGISTRATION

	; Register our frame handler for periodic updates
	call	Register_Frame_Handler

	pop	XBC
	pop	XIX
	pop	XIZ
	ret

; =============================================================================
; Register_Frame_Handler - Register our Frame_Handler for periodic callbacks
;
; Uses the workspace handler system to register for per-frame updates.
; =============================================================================
Register_Frame_Handler:
	push	XIZ
	push	XIX

	ld	XIZ, (WORKSPACE_PTR)
	ld	XIZ, (XIZ + 0E88h)	; WS_OFFSET_HANDLERS_B
	ld	XIX, (XIZ + 0108h)	; WS_HANDLER_B_FUNC1
	call	T, XIX

	pop	XIX
	pop	XIZ
	ret

; =============================================================================
; Frame_Handler - Called every frame by main firmware
;
; If game is not active: return immediately (let main firmware handle UI).
; If game is active: run one iteration of the game loop.
; If game exited (GAME_ACTIVE == 0 after C code ran): restore palette, return.
; =============================================================================
Frame_Handler:
	; Quick check: is game initialized?
	ld	A, (GAME_INITIALIZED)
	cp	A, 0
	jr	Z, .frame_exit

	; Update our copy of system ticks from main firmware's timer
	; The main firmware increments its tick counter via INTT1 ISR.
	; We just need to read it periodically for PRNG seeding.
	; (The C code references SYSTEM_TICKS for srand)

	; Check if game is active
	ld	A, (GAME_ACTIVE)
	cp	A, 0
	jr	Z, .frame_check_disk_menu

	; Game is active: call C game loop
	call	C_Game_Frame
	jr	.frame_exit

.frame_check_disk_menu:
	; Not active yet - the main firmware manages the DISK MENU.
	; When user selects our icon, the Alloc_Memory handler is called
	; and we need to detect when to activate the game.
	; For now, this path returns immediately.
	jr	.frame_exit

.frame_exit:
	ret

; =============================================================================
; Activate_Game - Called when DISK MENU selects our entry
;
; Sets up the game VGA palette, clears framebuffer, and starts the game.
; =============================================================================
Activate_Game:
	push	XWA
	push	XBC
	push	XDE

	; Save current palette
	call	Save_Palette

	; Load game palette
	call	Load_Game_Palette

	; Clear framebuffer
	lda	XWA, VIDEO_RAM_BASE
	ld	XBC, VRAM_SIZE
.clear_loop:
	ld	(XWA), 0
	inc	1, XWA
	dec	1, XBC
	jr	NZ, .clear_loop

	; Set game active
	ld	(GAME_ACTIVE), 1

	; Set up stack for C code
	LDA_XWA_IMM24 STACK_TOP
	ld	XSP, XWA

	; Clear C BSS
	call	Clear_C_BSS

	; Call C main()
	call	C_Main

	; Game has returned (QUIT state)
	; Restore palette
	call	Restore_Palette

	; Mark game as inactive
	ld	(GAME_ACTIVE), 0

	pop	XDE
	pop	XBC
	pop	XWA
	ret

; =============================================================================
; C_Game_Frame - Run one frame of the C game loop
;
; This is a thin wrapper that the frame handler calls when game is active.
; Since the C main() runs its own while loop, this is only used if we
; switch to a frame-callback-based architecture. For now, the game runs
; from Activate_Game → C_Main which contains the main loop.
; =============================================================================
C_Game_Frame:
	ret

; =============================================================================
; Clear_C_BSS - Zero the C BSS region
; =============================================================================
Clear_C_BSS:
	push	XWA
	push	XBC

	lda	XWA, C_BSS_START
	ld	XBC, C_BSS_END - C_BSS_START
.loop:
	ld	(XWA), 0
	inc	1, XWA
	dec	1, XBC
	jr	NZ, .loop

	pop	XBC
	pop	XWA
	ret

; =============================================================================
; VGA Palette routines
; =============================================================================

; VGA absolute addresses (combine shared offsets with VGA_IO_BASE)
VGA_DAC_WRITE_ADDR	EQU	VGA_IO_BASE + VGA_DAC_ADDR_WRITE	; 01703C8h
VGA_DAC_READ_ADDR	EQU	VGA_IO_BASE + 3C7h			; 01703C7h
VGA_DAC_DATA_ADDR	EQU	VGA_IO_BASE + VGA_DAC_DATA		; 01703C9h
VRAM_SIZE		EQU	76800		; 320 * 240

; Save current VGA palette (256 entries, 3 bytes each)
Save_Palette:
	push	XWA
	push	XBC
	push	XDE

	ld	(VGA_DAC_READ_ADDR), 0		; Start reading from index 0
	lda	XDE, SAVED_PALETTE
	ld	BC, 768				; 256 * 3
.save_loop:
	ld	A, (VGA_DAC_DATA_ADDR)
	ld	(XDE), A
	inc	1, XDE
	dec	1, BC
	jr	NZ, .save_loop

	pop	XDE
	pop	XBC
	pop	XWA
	ret

; Restore saved VGA palette
Restore_Palette:
	push	XWA
	push	XBC
	push	XDE

	ld	(VGA_DAC_WRITE_ADDR), 0	; Start writing from index 0
	lda	XDE, SAVED_PALETTE
	ld	BC, 768				; 256 * 3
.restore_loop:
	ld	A, (XDE)
	ld	(VGA_DAC_DATA_ADDR), A
	inc	1, XDE
	dec	1, BC
	jr	NZ, .restore_loop

	pop	XDE
	pop	XBC
	pop	XWA
	ret

; Load game palette from ROM data
; Palette data format: 256 entries * 3 bytes (R, G, B), 6-bit VGA values
Load_Game_Palette:
	push	XWA
	push	XBC
	push	XDE

	ld	(VGA_DAC_WRITE_ADDR), 0	; Start at index 0
	lda	XDE, PALETTE_DATA_ROM
	ld	BC, 768				; 256 * 3
.load_loop:
	ld	A, (XDE)
	ld	(VGA_DAC_DATA_ADDR), A
	inc	1, XDE
	dec	1, BC
	jr	NZ, .load_loop

	pop	XDE
	pop	XBC
	pop	XWA
	ret

; =============================================================================
; Control Panel Initialization
;
; Initialize the SC1 serial protocol for control panel communication.
; Must be called once before any cpanel queries.
; (SC1 hardware is already configured by boot_hw_init.asm)
; =============================================================================
Cpanel_Init:
	push	WA
	push	BC
	push	DE

	; Init command 1: 1F DA
	ld	B, 01Fh
	ld	C, 0DAh
	call	Cpanel_Query_Segment
	call	Cpanel_Delay

	; Init command 2: 1F 1A
	ld	B, 01Fh
	ld	C, 01Ah
	call	Cpanel_Query_Segment
	call	Cpanel_Delay

	; Init command 3: 1D 00
	ld	B, 01Dh
	ld	C, 000h
	call	Cpanel_Query_Segment
	call	Cpanel_Delay

	; Init command 4: DD 03
	ld	B, 0DDh
	ld	C, 003h
	call	Cpanel_Query_Segment
	call	Cpanel_Delay

	; Init command 5: 1E 80
	ld	B, 01Eh
	ld	C, 080h
	call	Cpanel_Query_Segment
	call	Cpanel_Delay

	; Clear serial interrupt flags
	ld	(INTCLR), 023h		; Clear INTTX1 pending
	ld	(INTCLR), 022h		; Clear INTRX1 pending

	pop	DE
	pop	BC
	pop	WA
	ret

; Cpanel_Send_Byte: Send/receive one byte via SC1
; Input: A = byte to send
; Output: A = byte received
Cpanel_Send_Byte:
	ld	(INTCLR), 022h		; Clear INTRX1 pending
	ld	(SC1BUF), A		; Start transfer
	push	DE
	ld	DE, 0			; Timeout counter
.wait:
	ld	A, (INTES1)
	and	A, 008h			; Test INTRX1 bit
	jr	NZ, .done
	djnz	DE, .wait
	pop	DE
	ld	A, 0
	ret
.done:
	pop	DE
	ld	A, (SC1BUF)
	ret

; Cpanel_Query_Segment: Query a control panel segment
; Input: B = command, C = segment
; Output: A = button bitmap
Cpanel_Query_Segment:
	ld	A, B
	call	Cpanel_Send_Byte
	ld	A, C
	call	Cpanel_Send_Byte
	ld	A, 0FFh
	call	Cpanel_Send_Byte
	ld	A, 0FFh
	call	Cpanel_Send_Byte
	ret

; Short delay for control panel init sequence
Cpanel_Delay:
	push	DE
	ld	DE, 08000h
.delay:
	djnz	DE, .delay
	pop	DE
	ret

; =============================================================================
; C Code Section
;
; All C code is compiled via the LLVM TLCS900 backend and converted from
; LLVM's assembly output to ASL format by tools/llvm_asm_to_asl.py.
;
; Compiled modules (all 6 C source files):
;   - extras.c:     rand, srand, memset, memcpy, malloc, calloc, free
;   - input.c:      input_read, random_number (with cpanel serial I/O)
;   - tiles.c:      draw_single_cell, draw_minefield_contents, draw_minefield,
;                   draw_scenario (the 8x8 tile rendering code)
;   - video.c:      set_tile, set_tile_full, highlight_current_cell,
;                   platform_init, platform_shutdown, idle_update, draw_scenario
;   - main.c:       main, gameplay_update, update_gameplay_input,
;                   game_over_update, title_screen_update, set_minefield_cell
;   - minefield.c:  init_minefield, free_minefield, reset_minefield,
;                   open_cell, open_block, count_surrounding_flags, maybe_game_won
; =============================================================================
	ORG 0281000h

; --- Compiled C code (extras.c, input.c, tiles.c) ---
	include "build/c_code_asl.asm"

; All C functions are now compiled from C via the LLVM TLCS900 backend.
; The following modules are included via c_code_asl.asm:
;   - extras.c:     rand, srand, memset, memcpy, malloc, calloc, free
;   - input.c:      input_read, random_number (with cpanel serial I/O)
;   - tiles.c:      draw_single_cell, draw_minefield_contents, draw_minefield, draw_scenario
;   - video.c:      set_tile, set_tile_full, highlight_current_cell, platform_init/shutdown, idle_update
;   - main.c:       main, gameplay_update, update_gameplay_input, game_over_update, title_screen_update
;   - minefield.c:  init_minefield, free_minefield, reset_minefield, open_cell, open_block, etc.

; =============================================================================
; Compiler runtime helpers
; =============================================================================

; __udivsi3: unsigned 32-bit division
; Args: XDE = dividend, XBC = divisor
; Returns: XDE = quotient (dividend / divisor)
__udivsi3:
	push	XWA
	push	XHL
	ld	XWA, XDE		; XWA = dividend
	ld	XHL, 0			; XHL = quotient
.udiv_loop:
	cp	XWA, XBC
	jr	C, .udiv_done		; dividend < divisor → done
	sub	XWA, XBC
	inc	4, XHL
	jr	.udiv_loop
.udiv_done:
	ld	XDE, XHL		; XDE = quotient
	pop	XHL
	pop	XWA
	ret

; __umodsi3: unsigned 32-bit modulo
; Args: XDE = dividend, XBC = divisor
; Returns: XDE = remainder (dividend % divisor)
__umodsi3:
	push	XWA
	ld	XWA, XDE		; XWA = dividend
.umod_loop:
	cp	XWA, XBC
	jr	C, .umod_done		; dividend < divisor → done
	sub	XWA, XBC
	jr	.umod_loop
.umod_done:
	ld	XDE, XWA		; XDE = remainder
	pop	XWA
	ret

; C_Main is the 'main' symbol from compiled C code (common/main.c)
C_Main		EQU main

; =============================================================================
; Read-Only Data Section
; =============================================================================

; Tile graphics data (generated by tools/generate_tiles.py)
TILES_DATA:
	binclude "build/tiles.bin"

; Palette data (generated by tools/generate_tiles.py)
PALETTE_DATA_ROM:
PALETTE_DATA:
	binclude "build/palette.bin"

; Icon data (27x27 pixels, displayed in DISK MENU)
ICON_DATA:
	binclude "build/icon.bin"

; =============================================================================
; Pad to 512KB
; =============================================================================
	ORG 02FFFFFh
	db 0FFh

; =============================================================================
; End of ROM
; =============================================================================
	end
