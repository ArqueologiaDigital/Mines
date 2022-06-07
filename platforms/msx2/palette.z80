.globl _set_palette
.globl _restore_palette

__msx1_palette:
; table:    index, RGB (#0..#7) ; name
	.db #0,    #0, #0, #0   ; transparent
	.db #1,    #0, #0, #0   ; black
	.db #2,    #1, #6, #1   ; bright green
	.db #3,    #3, #7, #3   ; light green
	.db #4,    #1, #1, #7   ; deep blue
	.db #5,    #2, #3, #7   ; bright blue
	.db #6,    #5, #1, #1   ; deep red
	.db #7,    #2, #6, #7   ; light blue
	.db #8,    #7, #1, #1   ; bright red
	.db #9,    #7, #3, #3   ; light red
	.db #10,   #6, #6, #1   ; bright yellow
	.db #11,   #6, #6, #3   ; pale yellow
	.db #12,   #1, #4, #1   ; deep green
	.db #13,   #6, #2, #5   ; purple
	.db #14,   #5, #5, #5   ; grey
	.db #15,   #7, #7, #7   ; white

_set_palette:
	ld hl, #2
	add hl, sp
	
	ld b, (hl)              ; color count

	inc hl
	ld a, (hl)
	inc hl
	ld h, (hl)
	ld l, a                 ; color table address

_color_loop:
	di
	ld a, (hl)
	inc hl
	out (#0x99), a          ; color number
	ld a, #128 + #16
	out (#0x99), a
	ld a, (hl)              ; red
	inc hl
	inc hl
	sla a
	sla a
	sla a
	sla a                   ; bits 4-7
	ld c, a
	ld a, (hl)              ; blue bits 0-3
	dec hl
	or c
	out (#0x9A), a          ; write components r&b
	ld a,(hl)               ; green bits 0-3
	inc hl
	inc hl
	out (#0x9A), a          ; write component g
	ei

	ld a, b
	dec a                   ; check number of remaining colors
	djnz _color_loop

	ret

_restore_palette:
	ld hl, #__msx1_palette
	ld b, #0x10
	jr _color_loop

