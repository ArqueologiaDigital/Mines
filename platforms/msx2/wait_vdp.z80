.globl _wait_vdp

_wait_vdp::
	ld a, #2
	out (#0x99), a          ; define VDP status register 1

	ld a, #128 + #15
	out (#0x99), a          ; tell VDP to read status register 1 (R#15)
	in a, (#0x99)           ; read it

	and #1
	jr nz, _wait_vdp        ; wait if active

	xor a
	out (#0x99), a
	ld a, #128 + #15
	out (#0x99), a          ; reset R#15 to avoid system crash
	ret

