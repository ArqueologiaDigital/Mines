#!/bin/sh
srec_cat mines.dat -binary -o mines.tmp -asm -hexadecimal_style
sed s/DB/\.db/ mines.tmp > mines.z80
rm mines.tmp
