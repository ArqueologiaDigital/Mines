#!/bin/sh
# MINES.SC5 generated from mines.png by https://msx.jannone.org/conv/
dd if=MINES.SC5 of=mines.dat skip=7 bs=1
srec_cat mines.dat -binary -o mines.c -c-array mines_data -hexadecimal_style
rm mines.dat
