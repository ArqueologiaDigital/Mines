; void put_sprite_colors(uint8_t* src, uint8_t index) SDCCCALL0

.globl _put_sprite_colors
.globl _write_vram

SPRITE_COLOR_TABLE = 0x7400

_put_sprite_colors::
	ld hl, #2
	add hl, sp

	ld e, (hl)
	inc hl
	ld d, (hl)      ; de <- source color data address

	inc hl
	ld l, (hl)
	ld h, #0        ; hl <- sprite name table index

	add hl, hl
	add hl, hl
	add hl, hl
	add hl, hl      ; hl <- hl * 16

	ld bc, #SPRITE_COLOR_TABLE
	add hl, bc      ; hl <- SPRITE_COLOR_TABLE + 16 * sprite_index

	push de         ; src
	ld bc, #16      ; copy 16 bytes of sprite colors (1 color per line)
	push bc         ; len
	push hl         ; dst
	call _write_vram
	pop af
	pop af
	pop af

	ret
