;; customized crt0.s
.module crt0
.globl  _main

.area	_HOME
.area	_CODE
.area	_DATA


.area   _HEADER (ABS)
;; Cartridge header
.org 	0
	.ascii 'g GCE '				;; wont boot without this!
	.ascii "2022"				;; year of release
	.db 0x80
	.dw 0x0F43					;; music played at boot
	.dw 0xF848					;; title position 
	.dw 0x20E0					;; title size
	.ascii "MINES"				;; Game Title (shows up on screen during boot)
	.db 0x80, 0x00

init:
	;; IMPLEMENT-ME!
	;;call gsinit  ;; Initialise global variables
	jsr _main
	jmp	init

.area	_CODE

_exit::
;;	jmp init

;; .area   _GSINIT
gsinit::
	;; IMPLEMENT-ME!
	;;ld bc, #l__INITIALIZER
	;;ld a, b
	;;or a, c
	;;jr Z, gsinit_next
	;;ld de, #s__INITIALIZED
	;;ld hl, #s__INITIALIZER
	;;ldir
gsinit_next:

;; .area   _GSFINAL
;;	ret

