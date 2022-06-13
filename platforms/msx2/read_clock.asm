.globl _read_clock

EXTROM = 0x015f
REDCLK = 0x01f5

_read_clock_byte:
	ld ix, #REDCLK
	call EXTROM
	ld l, a

	inc c                   ; read next register (4MSB)
	ld ix, #REDCLK
	call EXTROM
	sla a
	sla a
	sla a
	sla a
	or l                    ; result in a

	ret

_read_clock::
	ld c, #0b00000;         ; block 0, register 0-1 (seconds)
	call _read_clock_byte
	push af

	ld c, #0b00010;         ; block 0, register 2-3 (minutes)
	call _read_clock_byte
	push af

	ld c, #0b00100;         ; block 0, register 4-5 (hours)
	call _read_clock_byte
	ld d, #0
	ld e, a

	pop af
	ld h, a

	pop af
	ld l, a

	ret

