#include "../../common/minefield.h"

void platform_init()
{
	//TODO: Implement-me!
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
