.globl _xorshift
.globl _seed

_set_random_seed::
    ld hl, #2
    add hl, sp
    ld iy, #_seed

    ld a, (hl)
    or #1
    ld 0(iy), a

    inc hl
    ld a, (hl)
    ld 1(iy), a

    ret

_xorshift::
	ld hl, (_seed)

	ld a, h
	rra
	ld a, l
	rra
	xor h
	ld h, a
	ld a, l
	rra
	ld a, h
	rra
	xor l
	ld l, a
	xor h
	ld h, a

	ld (_seed), hl

	ret

.area _DATA

_seed: .dw 1    ; value must not be zero

