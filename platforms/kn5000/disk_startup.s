; =============================================================================
; disk_startup.s - Mines for KN5000 (Disk App, loaded by App Loader)
; =============================================================================
; Simplified startup for running as a disk application loaded by the
; HDAE5000 App Loader. No XAPR header, no handler registration.
;
; The App Loader loads APP.BIN to 0x208000 and calls the entry point.
; This startup switches to the app's own stack, initializes the C runtime,
; runs main(), and returns to the App Loader when done.
;
; yield_to_firmware() uses the same SP save addresses as the App Loader
; (SAVED_SP=0x200044, APP_SAVED_SP=0x200048), forming a seamless coroutine
; with the App Loader's Frame_Handler.
; =============================================================================

; =============================================================================
; Hardware Constants
; =============================================================================
.equ VIDEO_RAM_BASE,    0x1A0000
.equ VRAM_SIZE,         76800

; =============================================================================
; Shared addresses (MUST match App Loader's startup.s)
; =============================================================================
.equ SAVED_SP,          0x200044        ; Firmware SP (set by App Loader Frame_Handler)
.equ APP_SAVED_SP,      0x200048        ; App SP (used by yield/resume coroutine)

; =============================================================================
; App-private RAM (in gap between loader data and app load area)
; =============================================================================
.equ CALLER_SAVED_SP,   0x207FF0        ; App Loader's SP saved here on entry
.equ STACK_TOP,         0x27F000        ; App stack (grows down)

; =============================================================================
; Exports (symbols referenced by C code)
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
; Entry Point (.startup section, placed at start of binary)
; =============================================================================
.section .startup, "ax", @progbits

.globl _start
_start:
        ; Save the App Loader's stack pointer (includes return address)
        ld      xwa, xsp
        ld      (CALLER_SAVED_SP), xwa

        ; Switch to app's own stack
        ld      xsp, STACK_TOP

        ; Initialize C runtime (BSS only — data is already in place)
        call    Clear_C_BSS

        ; Run the game
        call    main

        ; Game exited — restore App Loader's stack and return
        ld      xwa, (CALLER_SAVED_SP)
        ld      xsp, xwa
        ret

; =============================================================================
; yield_to_firmware
; =============================================================================
; Cooperative multitasking: saves app state, returns control to firmware.
; Uses the same SP addresses as the App Loader's coroutine mechanism.
; On next frame, App Loader's Frame_Handler resumes execution here.
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
        ld      (APP_SAVED_SP), xwa
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
; Clear_C_BSS - Zero uninitialized data
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
; clear_vram - Clear display framebuffer
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
; Runtime helpers (required by LLVM-generated code)
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
; Read-only Data
; =============================================================================
.section .rodata, "a", @progbits
TILES_DATA:     .incbin "build/tiles.bin"
PALETTE_DATA:   .incbin "build/palette.bin"

; =============================================================================
; BSS Variables (provided for C code compatibility)
; =============================================================================
.section .bss, "aw", @nobits
GAME_ACTIVE:    .zero 4
SYSTEM_TICKS:   .zero 4
