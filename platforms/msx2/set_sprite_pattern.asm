; set_sprite_pattern(uint8_t *src, uint8_t pattern_no) SDCCCCALL0

.globl _set_sprite_pattern
.globl _write_vram

SPRITE_PATTERN_TABLE = 0x7800

_set_sprite_pattern::
	ld hl, #2
	add hl, sp

	ld e, (hl)
	inc hl
	ld d, (hl)              ; de <- sprite pattern address in memory

	inc hl
	ld l, (hl)
	ld h, #0                ; hl <- sprite pattern table number

	add hl, hl
	add hl, hl
	add hl, hl
	add hl, hl
	add hl, hl
	ld bc, #SPRITE_PATTERN_TABLE
	add hl, bc              ; hl <- SPRITE_PATTERN_TABLE + 32 * pattern_no

	push de                 ; src
	ld bc, #32
	push bc                 ; len
	push hl                 ; dst
	call _write_vram        ; copy 32 bytes of sprite pattern from hl to VRAM
	pop af
	pop af
	pop af

	ret

