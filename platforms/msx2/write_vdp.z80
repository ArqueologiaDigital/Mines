.globl _write_vdp

WRITEVDP = 0x0047

_write_vdp::
	ld hl, #2
	add hl, sp

	ld c, (hl)

	inc hl
	ld b, (hl)

	call WRITEVDP
	ret

