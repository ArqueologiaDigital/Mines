.globl _lmmm_op

;*******************************************************************
;*  LMMM (high speed Logical Move from VRAM to VRAM) with operator
;*  copy a rectangle from 'Source' VRAM to 'Destination' VRAM
;*  sx: X coordinate of pixel source (left side)
;*  sy: Y coordinate of pixel source (upper side)
;*  dx: X coordinate of pixel destination (left side)
;*  dy: Y coordinate of pixel destination (upper side)
;*  width: number of pixels to the right
;*  height: number of pixels to the bottom
;*  op: logical operation to apply between src pixel and dest pixel
;*******************************************************************

_lmmm_op::
	di                      ; disable interrupts
	call _wait_vdp          ; check if previous VDP command finished or wait if necessary

	ld a, #0 + #32          ; auto-increment is active
	out (#0x99), a          ; define VDP register to write to (R#32)

	ld a, #128 + #17
	out (#0x99), a          ; R#17 is used to indirectly access register #32

	ld c, #0x9b             ; VDP port for writing

	ld iy, #2
	add iy, sp

	ld a, 0(iy)
	out (c), a              ; sx: low byte (R#32)
	ld a, 1(iy)
	out (c), a              ; sx: high byte (R#33)
	ld a, 2(iy)
	out (c), a              ; sy: low byte (R#34)
	ld a, 3(iy)
	out (c), a              ; sy: high byte (R#35)
	ld a, 4(iy)
	out (c), a              ; dx: low byte (R#36)
	ld a, 5(iy)
	out (c), a              ; dx: high byte (R#37)
	ld a, 6(iy)
	out (c), a              ; dy: low byte (R#38)
	ld a, 7(iy)
	out (c), a              ; dy: low byte (R#39)
	ld a, 8(iy)
	out (c), a              ; width: low byte (R#40)
	ld a, 9(iy)
	out (c), a              ; width: high byte (R#41)
	ld a, 10(iy)
	out (c), a              ; height: low byte (R#42)
	ld a, 11(iy)
	out (c), a              ; height: high byte (R#43)

	xor a
	out (c), a              ; spurious write operation: update register counter (R#44)
	out (c), a              ; VRAM->VRAM (bits 0#5, 0#4), top->bottom (bit 0#3), left->right (bit 0#2) (R#45)

	ld a, 12(iy)
	or a, #0b10010000       ; LMMM command (1001) + IMP op (parameter)
	out (c), a              ; engage! (R#46)
	ei                      ; enable interrupts

	ret
