; uint8_t keyboard_read_row(uint8_t row) __z88dk_fastcall
.globl _read_keyboard_row

P_PPI_C = 0xaa
P_PPI_B = 0xa9

_read_keyboard_row::
    in      a, (#P_PPI_C)
    and     #0xf0               ; keep PPI data constant...
    or      l                   ; ...but change row
    out     (#P_PPI_C), a
    in      a, (#P_PPI_B)       ; read row data
    ld      l, a
    ret
