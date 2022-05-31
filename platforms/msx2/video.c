#include <stdint.h>
#include <string.h>
#include "../../common/minefield.h"


extern uint8_t key_buffer_data[256];


void platform_init()
{
	memset(key_buffer_data, 0xff, 256);
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
