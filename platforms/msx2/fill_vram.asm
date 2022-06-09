.globl _fill_vram

BIGFIL = 0x016b

_fill_vram::
	ld hl, #2
	add hl, sp

	ld e, (hl)      ; e = byte

	inc hl
	ld c, (hl)
	inc hl
	ld b, (hl)      ; bc = length

	inc hl
	ld a, (hl)
	inc hl
	ld h, (hl)
	ld l, a         ; hl = dst

	ld a, e         ; a = e = byte

	jp BIGFIL

