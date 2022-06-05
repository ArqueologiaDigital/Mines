#include <stdint.h>
#include <string.h>
#include "msx.h"
#include "minefield.h"
#include "input.h"

void write_vdp(uint8_t reg, uint8_t data) SDCCCALL;

void write_vm_64k(uint8_t *dst, uint16_t len, uint8_t *src) SDCCCALL;

uint8_t* mines_data;

void platform_init()
{
    // Set SCREEN5
    write_vdp(0, 6);
    write_vdp(1, 60);

    // Move board and mines to VRAM
    write_vm_64k((uint8_t*) 0x0, 0x1800, mines_data);

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


void idle_loop()
{
}


void shutdown()
{
	//TODO: Implement-me!
}
