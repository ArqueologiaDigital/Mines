.globl _write_vram

NSTWRT = 0x0171
VDP.DW = 0x07

_write_vram::
	ld hl, #2
	add hl, sp
	ld e, (hl)	; de = dst
	inc hl
	ld d, (hl)

	inc hl		; bc = length
	ld c, (hl)
	inc hl
	ld b, (hl)

	inc hl		; hl = src
	ld a, (hl)
	inc hl
	ld h, (hl)
	ld l, a

	ex de, hl	; hl = dst, de = src
	push bc		; save length
	call NSTWRT
	ex de, hl	; hl = src, de = dst
	pop de		; de = length

	ld a, (VDP.DW)
	ld c, a

	ld b, e
	ld a, d
	inc e
	dec e
	jr z, loop
	inc a

loop:
	outi
	jp nz, loop
	dec a
	jp nz, loop

	ret
