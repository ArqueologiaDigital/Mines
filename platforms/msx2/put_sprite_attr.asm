; void put_sprite_attr(struct sprite_attr* src, uint8_t index) SDCCCALL0

.globl _put_sprite_attr
.globl _write_vram

SPRITE_ATTR_TABLE = 0x7600

_put_sprite_attr::
	ld hl, #2
	add hl, sp

	ld e, (hl)
	inc hl
	ld d, (hl)      ; de <- sprite attribute address

	inc hl
	ld l, (hl)      ; l <- sprite name table index

	sla l
	sla l
	ld h, #0
	ld bc, #SPRITE_ATTR_TABLE
	add hl, bc      ; hl <- SPRITE_ATTR_TABLE + 4 * index

	push de         ; src
	ld bc, #3       ; length of 3: y, x, pattern_no
	push bc         ; len
	push hl         ; dst
	call _write_vram
	pop af
	pop af
	pop af

	ret
