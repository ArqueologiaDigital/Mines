; void read_mouse(struct mouse* mouse, uint8_t source) __sdcccall(0)

.globl read_mouse

REGWTP = 0xA0           ; register write port
VALWTP = 0xA1           ; value write port
VALRDP = 0xA2           ; value read port
MOUSE1 = 0x1310
MOUSE2 = 0x6c20
SHORT_WAIT = 10
LONG_WAIT = 30
LONGER_WAIT = 40
IO_REG1 = 14
IO_REG2 = 15

_read_mouse::
        push ix
        ld ix, #2
        add ix, sp

        ld l, 2(ix)
        ld h, 3(ix)
        ld a, 4(ix)

        ld de, #MOUSE1                  ; DE = 01310h for mouse in port 1 (D = 00010011b, E = 00010000b)
        and #02
        jr z, read_mouse
        ld de, #MOUSE2                  ; DE = 06C20h for mouse in port 2 (D = 01101100b, E = 00100000b)

read_mouse:
        di
        ld b, #LONG_WAIT                ; first delay is the longest
        call get_mouse_data0            ; read bits 7-4 of the x offset
        and #0x0F
        rlca
        rlca
        rlca
        rlca                            ; adjust x offset's higher bits
        ld c, a                         ; save temp data
        call get_mouse_data             ; read bits 3-0 of the x offset
        and #0x0F
        or c                            ; restore temp data
        ld (hl), a                      ; store combined x offset
        inc hl
        call get_mouse_data             ; read bits 7-4 of the y offset
        and #0x0F
        rlca
        rlca
        rlca
        rlca                            ; adjust y offset's higher bits
        ld c, a                         ; save temp data
        call get_mouse_data             ; read bits 3-0 of the y offset
                                        ; can read mouse button bit 4 = left button / bit 5 = right button
        ld b, a
        and #0x0F
        or c                            ; restore temp data
        ld (hl), a                      ; save to mouse data struct 1/3
        ld a, b
        and #0x10
        rlca
        rlca
        rlca
        rlca
        inc hl
        ld (hl), a                      ; save to mouse data struct 2/3
        ld a, b
        and #0x20
        rlca
        rlca
        rlca
        rlca
        rlca
        inc hl
        ld (hl), a                      ; save to mouse data struct 3/3

        ei

        ld b, #LONGER_WAIT
        call wait

        pop ix
        ret

get_mouse_data:
        ld b, #SHORT_WAIT
get_mouse_data0:
        ld a, #IO_REG2
        out (#REGWTP), a                ; read PSG register 15 for mouse

        in a, (#VALWTP)
        and #0x80                       ; preserve LED code/Kana state
        or d                            ; mouse1 x0010011b / mouse2 x1101100b
        out (#VALWTP), a
        xor e                           ; first pass pin8 is 1, at second pass pin8 is 0 and so on
        ld d, a                         ; store reversed pin 8 state
            
        call wait                       ; wait before read : 100+/-150us for first read
                                        ;                     50+/-150us for next reads
                
        ld a, #IO_REG1
        out (#REGWTP), a                ; select first IO register
        in a, (#VALRDP)                 ; read actual data
        ret
wait:
        ld a, b                         ; save counter
wait_loop0:
        djnz wait_loop0
        .db #0xed, #0x55                ; go back if on Z80 (RETN on Z80, NOP on R800)
        rlca
        rlca
        ; add a, b
        ld b, a                         ; restore counter
wait_loop2:                             ; R800 extra wait 1/2
        djnz wait_loop2
        ld b, a                         ; restore counter
wait_loop3:                             ; R800 extra wait 2/2
        djnz wait_loop3
        ret
