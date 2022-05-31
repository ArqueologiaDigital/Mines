#include <assert.h>
#include <stdbool.h>
#include "ports.h"
#include "input.h"


#define KB_MAX_MATRIX_ROW 11
uint8_t keyboard_matrix_row[KB_MAX_MATRIX_ROW] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
uint8_t old_keyboard_matrix_row[KB_MAX_MATRIX_ROW] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

/* bool key_buffer_empty = true; */
uint8_t key_buffer_head = 0;
uint8_t key_buffer_tail = 0;
uint8_t key_buffer_data[256];


uint8_t keyboard_read_row(uint8_t row) __z88dk_fastcall __naked
{
	assert(row <= 10);
	__asm
		in		a, (P_PPI_C)
		and		#0xf0           /* keep PPI data constant... */
		or		l               /* ...but change row */
		out		(P_PPI_C), a
		in		a, (P_PPI_B)    /* read row data */
		ld		l, a
		ret
	__endasm;
}


bool keyboard_read()
{
	bool key_pressed = false;

	for (uint8_t row = 0; row <= KB_MAX_MATRIX_ROW; ++row)
	{
		old_keyboard_matrix_row[row] = keyboard_matrix_row[row];
		keyboard_matrix_row[row] = keyboard_read_row(row);

		if (keyboard_matrix_row[row] != old_keyboard_matrix_row[row]) {
			bool found;

			for (uint8_t b = 0; b < 8; ++b) {
				found = (keyboard_matrix_row[row] & (1 << b)) == 0;
				if (!found) continue;
				key_buffer_data[key_buffer_tail++] = (b << 4) | row;
				key_pressed |= true;
				/* key_buffer_empty = false; */
			}
		}
	}

	return key_pressed;
}


uint8_t input_read(uint8_t source)
{
	source;
	bool key_pressed = false;

	while (!key_pressed)
		key_pressed = keyboard_read();

	return key_buffer_data[++key_buffer_head];
}

uint8_t MATRIX_KEY_2_COLUMN[8][12] = {
	"08'CKS\000\000 \0005",
	"19`DLT\000\000\000\0006",
	"2-,EMU\000\x1c\000\0007",
	"3=.FNV\000\x08\00008",
	"4\\/GOW\000\000\00019",
	"5[$HPX\000\000\0002-",
	"6]AIQY\000\000\0003,",
	"7;BJRZ\000\n\0004."
};

uint8_t get_raw_ch()
{
	uint8_t key;

	do {
		key = input_read(KEYBOARD);
	} while (key == MINE_INPUT_IGNORED);

	uint8_t row = key & 0x0f;
	return MATRIX_KEY_2_COLUMN[key >> 4][row];
}

int random_number(int min_num, int max_num)
{
	// TODO: Implement-me!
	min_num; max_num;
	return 0;
}
