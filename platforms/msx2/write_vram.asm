; void write_vram(uint16_t dst, uint16_t len, uint8_t *src) SDCCCALL0

.globl _write_vram

NSTWRT = 0x0171
VDP.DW = 0x07

_write_vram::
	ld hl, #2
	add hl, sp

	ld e, (hl)
	inc hl
	ld d, (hl)      ; de <- dst

	inc hl
	ld c, (hl)
	inc hl
	ld b, (hl)      ; bc <- length

	inc hl
	ld a, (hl)
	inc hl
	ld h, (hl)
	ld l, a         ; hl <- src

	ex de, hl       ; hl <- dst, de <- src
	call NSTWRT
	ex de, hl       ; hl <- src, de <- dst
	ld d, b
	ld b, c         ; db <- len

	ld a, (VDP.DW)
	ld c, a         ; OUTI uses port in c

	ld a, d         ; ab <- len
	inc b
	dec b
	jr z, loop      ; OUTI and cycle b if zero
	inc a           ; preserve value of a in iteration

loop:
	outi            ; read byte from (hl) and OUTI in c
	jp nz, loop     ; loop while b > 0
	dec a           ; b "borrowed" from a
	jp nz, loop     ; loop while a > 0

	ret
