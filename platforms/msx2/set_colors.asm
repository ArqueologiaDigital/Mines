.globl _set_colors

CHGCLR = 0x0062
FORCLR = 0xf3e9
BAKCLR = 0xf3ea
BDRCLR = 0xf3eb

_set_colors::
	ld hl, #2
	add hl, sp

	ld a, (hl)
	inc hl
	ld (FORCLR), a
	ld a, (hl)
	inc hl
	ld (BAKCLR), a
	ld a, (hl)
	ld (BDRCLR), a
	call CHGCLR
	ret
