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
.equ DISP_DISABLE_FLAG, 0x0D53          ; Bit 3: disable firmware display updates

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
.equ HANDLER_WORKSPACE, 0x200100        ; 64 bytes workspace for handler lifecycle
.equ STACK_TOP,        0x203000

; =============================================================================
; Exports (symbols referenced by C code in all_c.o)
; =============================================================================
.globl GAME_ACTIVE
.globl SYSTEM_TICKS
.globl TILES_DATA
.globl PALETTE_DATA
.globl yield_to_firmware
.globl clear_vram
.globl __udivsi3
.globl __umodsi3

; =============================================================================
; Code Section (.startup, placed at 0x280000 by linker script)
; =============================================================================
.section .startup, "ax", @progbits

; --- XAPR Header (8 bytes at offset 0x00) ---
; Bytes 0-3: "XAPR" magic (checked by main firmware)
; Bytes 4-7: version/flags metadata (not a pointer)
        .ascii  "XAPR"
        .byte   0x34, 0xA1, 0x2F, 0x00

; --- Entry Point 1: Boot_Init (offset 0x08, 8-byte slot) ---
        jp      Boot_Init
        ret
        .byte   0x00, 0x00, 0x00

; --- Entry Point 2: Frame_Handler (offset 0x10, 8-byte slot) ---
        jp      Frame_Handler
        ret
        .byte   0x00, 0x00, 0x00

; --- Entry Point 3: Unused (offset 0x18, 8-byte slot) ---
        ret
        .byte   0x00, 0x00, 0x00

; --- Entry Point 4: Unused (offset 0x1C, 4-byte slot) ---
        ret
        .byte   0x00, 0x00, 0x00

; =============================================================================
; HANDLER_REGISTRATION
; =============================================================================
HANDLER_REGISTRATION:
        push    xix
        push    xiz

        ; Get table A pointer
        ld      xiz, (WORKSPACE_PTR)
        add     xiz, 0x0E0A
        ld      xiz, (xiz)

        ; Build 14-byte param block in RAM at PARAM_BLOCK
        ld      xhl, PARAM_BLOCK
        ld      xwa, 0x01600004
        ld      (xhl), xwa

        add     xhl, 4
        push    xiz
        add     xiz, 0x0168
        ld      xwa, (xiz)
        pop     xiz
        ld      (xhl), xwa

        add     xhl, 4
        ld      xwa, 13
        ld      (xhl), xwa

        add     xhl, 2
        ld      xwa, MINES_RECORD_TABLE
        ld      (xhl), xwa

        push    xiz
        add     xiz, 0x00E4
        ld      xix, (xiz)
        pop     xiz

        ld      xwa, 0x016A
        ld      xbc, PARAM_BLOCK
        call    (xix)

        ; Reload table A
        ld      xiz, (WORKSPACE_PTR)
        add     xiz, 0x0E0A
        ld      xiz, (xiz)

        push    xiz
        add     xiz, 0x02C4
        ld      xix, (xiz)
        pop     xiz

        ld      xwa, 0x00600002
        call    (xix)

        ld      xwa, MENU_NAME
        ld      (xhl + 0x2A), xwa

        ld      xwa, 176
        ld      (xhl + 0x32), xwa

        ld      xwa, 0x016A0005
        ld      (xhl), xwa

        ld      (HANDLER_SLOT), xhl

        pop     xiz
        pop     xix
        ret

; =============================================================================
; Mines_Handler
; =============================================================================
Mines_Handler:
        push    xix
        push    xiz

        ; Check if game active
        push    xwa
        push    xhl
        ld      xhl, GAME_ACTIVE
        ld      xwa, (xhl)
        and     xwa, 0xFF
        cp      xwa, 0
        pop     xhl
        pop     xwa
        jr      nz, .Lmh_done

        ; Check activation events
        ld      xix, 0x01C00008
        cp      xbc, xix
        jr      z, .Lmh_activate
        ld      xix, 0x01E0009C
        cp      xbc, xix
        jr      z, .Lmh_activate

        jr      .Lmh_delegate

.Lmh_activate:
        push    xwa
        push    xhl
        ld      xhl, GAME_ACTIVE
        ld      xwa, 1
        ld      (xhl), xwa

        ; Disable firmware display updates (SET bit 3 to 0 via mask)
        ld      xhl, DISP_DISABLE_FLAG
        ld      a, (xhl)
        and     a, 0xF7
        ld      (xhl), a

        pop     xhl
        pop     xwa
        jr      .Lmh_done

.Lmh_delegate:
        ld      xiz, (WORKSPACE_PTR)
        add     xiz, 0x0E0A
        ld      xiz, (xiz)
        add     xiz, 0x00DC
        ld      xix, (xiz)
        call    (xix)

.Lmh_done:
        pop     xiz
        pop     xix
        ret

; =============================================================================
; Alloc_Memory_Icon
; =============================================================================
Alloc_Memory_Icon:
        push    xix
        ld      xwa, xbc
        and     xwa, 0xFF
        cp      xwa, 0xA1
        jr      z, .Lreturn_data_ptr
        cp      xwa, 0xA2
        jr      z, .Lreturn_width
        cp      xwa, 0xA3
        jr      z, .Lreturn_height
        jr      .Licon_done
.Lreturn_data_ptr:
        ld      xwa, ICON_DATA
        jr      .Licon_done
.Lreturn_width:
        ld      xwa, 27
        jr      .Licon_done
.Lreturn_height:
        ld      xwa, 27
.Licon_done:
        pop     xix
        ret

; =============================================================================
; Dummy_Return
; =============================================================================
Dummy_Return:
        ret

; =============================================================================
; Boot_Init
; =============================================================================
Boot_Init:
        ld      (WORKSPACE_PTR), xwa
        call    HANDLER_REGISTRATION

        ret

; =============================================================================
; Frame_Handler
; =============================================================================
Frame_Handler:
        push    xwa
        push    xbc
        push    xde
        push    xhl
        push    xix
        push    xiy
        push    xiz

        ld      xhl, GAME_ACTIVE
        ld      xwa, (xhl)
        and     xwa, 0xFF
        cp      xwa, 0
        jrl     z, .Lframe_done

        ld      xhl, GAME_INITIALIZED
        ld      xwa, (xhl)
        and     xwa, 0xFF
        cp      xwa, 0
        jrl     nz, .Lresume_game

        ; First activation
        ld      xwa, xsp
        ld      (SAVED_SP), xwa
        ld      xsp, STACK_TOP
        ld      xhl, GAME_INITIALIZED
        ld      xwa, 1
        ld      (xhl), xwa
        call    Copy_C_Data
        call    Clear_C_BSS
        call    main

        ; Exit
        call    clear_vram
        ; Re-enable display (SET bit 3 to 1 via mask)
        ld      xhl, DISP_DISABLE_FLAG
        ld      a, (xhl)
        or      a, 0x08
        ld      (xhl), a
        
        ld      xhl, GAME_ACTIVE
        ld      xwa, 0
        ld      (xhl), xwa
        ld      xhl, GAME_INITIALIZED
        ld      (xhl), xwa
        ld      xwa, (SAVED_SP)
        ld      xsp, xwa
        jr      .Lframe_done

.Lresume_game:
        ld      xwa, xsp
        ld      (SAVED_SP), xwa
        ld      xwa, (GAME_SAVED_SP)
        ld      xsp, xwa
        pop     xiz
        pop     xiy
        pop     xix
        pop     xhl
        pop     xde
        pop     xbc
        pop     xwa
        ret

.Lframe_done:
        pop     xiz
        pop     xiy
        pop     xix
        pop     xhl
        pop     xde
        pop     xbc
        pop     xwa
        ret

; =============================================================================
; yield_to_firmware
; =============================================================================
yield_to_firmware:
        push    xwa
        push    xbc
        push    xde
        push    xhl
        push    xix
        push    xiy
        push    xiz
        ld      xwa, xsp
        ld      (GAME_SAVED_SP), xwa
        ld      xwa, (SAVED_SP)
        ld      xsp, xwa
        pop     xiz
        pop     xiy
        pop     xix
        pop     xhl
        pop     xde
        pop     xbc
        pop     xwa
        ret

; =============================================================================
; Copy_C_Data
; =============================================================================
Copy_C_Data:
        push    xwa
        push    xbc
        push    xde
        push    xhl
        ld      xde, __data_load
        ld      xhl, __data_start
        ld      xbc, __data_size
        cp      xbc, 0
        jr      z, .Lcopy_done
.Lcopy_loop:
        ld      xwa, (xde)
        ld      (xhl), xwa
        add     xde, 4
        add     xhl, 4
        sub     xbc, 4
        jr      nz, .Lcopy_loop
.Lcopy_done:
        pop     xhl
        pop     xde
        pop     xbc
        pop     xwa
        ret

; =============================================================================
; Clear_C_BSS
; =============================================================================
Clear_C_BSS:
        push    xwa
        push    xbc
        push    xde
        ld      xwa, __bss_start
        ld      xbc, __bss_size
        ld      xde, 0
        cp      xbc, 0
        jr      z, .Lbss_done
.Lbss_loop:
        ld      (xwa), xde
        add     xwa, 4
        sub     xbc, 4
        jr      nz, .Lbss_loop
.Lbss_done:
        pop     xde
        pop     xbc
        pop     xwa
        ret

; =============================================================================
; clear_vram
; =============================================================================
clear_vram:
        push    xwa
        push    xbc
        push    xhl
        ld      xhl, VIDEO_RAM_BASE
        ld      xwa, 0
        ld      xbc, VRAM_SIZE / 4
.Lclear_loop:
        ld      (xhl), xwa
        add     xhl, 4
        sub     xbc, 1
        jrl     nz, .Lclear_loop
        pop     xhl
        pop     xbc
        pop     xwa
        ret

; =============================================================================
; Runtime helpers
; =============================================================================
__udivsi3:
        push    xwa
        push    xhl
        ld      xwa, xde
        ld      xhl, 0
.Ludiv_loop:
        cp      xwa, xbc
        jr      c, .Ludiv_done
        sub     xwa, xbc
        inc     1, xhl
        jr      .Ludiv_loop
.Ludiv_done:
        ld      xde, xhl
        pop     xhl
        pop     xwa
        ret

__umodsi3:
        push    xwa
        ld      xwa, xde
.Lumod_loop:
        cp      xwa, xbc
        jr      c, .Lumod_done
        sub     xwa, xbc
        jr      .Lumod_loop
.Lumod_done:
        ld      xde, xwa
        pop     xwa
        ret

; =============================================================================
; Data
; =============================================================================
.section .rodata, "a", @progbits
TILES_DATA:     .incbin "build/tiles.bin"
PALETTE_DATA:   .incbin "build/palette.bin"
ICON_DATA:      .incbin "build/icon.bin"
MENU_NAME:      .asciz  "Mines Game"

MINES_RECORD_TABLE:
        .long   Dummy_Return, 0xFFFFFFFF, 0, 0, 0, 0
        .long   Dummy_Return, 0xFFFFFFFF, 0, 0, 0, 0
        .long   Dummy_Return, 0xFFFFFFFF, 0, 0, 0, 0
        .long   Dummy_Return, 0xFFFFFFFF, 0, 0, 0, 0
        .long   Dummy_Return, 0xFFFFFFFF, 0, 0, 0, 0
        .long   Mines_Handler
        .long   0x0160001D
        .byte   0x36, 0x00, 0x00, 0x00
        .long   MENU_NAME, 0, HANDLER_WORKSPACE
        .long   Dummy_Return, 0xFFFFFFFF, 0, 0, 0, 0
        .long   Dummy_Return, 0xFFFFFFFF, 0, 0, 0, 0
        .long   Dummy_Return, 0xFFFFFFFF, 0, 0, 0, 0
        .long   Dummy_Return, 0xFFFFFFFF, 0, 0, 0, 0
        .long   Dummy_Return, 0xFFFFFFFF, 0, 0, 0, 0
        .long   Dummy_Return, 0xFFFFFFFF, 0, 0, 0, 0
        .long   Dummy_Return, 0xFFFFFFFF, 0, 0, 0, 0
