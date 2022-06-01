#include <stdint.h>
#include <string.h>
#include "minefield.h"
#include "input.h"


void platform_init()
{
    // TODO: set SCREEN5
    while (1)
        input_read(KEYBOARD);
}


void putchar(char c) __z88dk_fastcall
{
	c;
	__asm
		ld a, l
		call 0x00A2     ;BIOS call for display the caracter
	__endasm;
}


void draw_minefield(minefield* mf)
{
	//TODO: Implement-me!
	mf;
}


void shutdown()
{
	//TODO: Implement-me!
}
