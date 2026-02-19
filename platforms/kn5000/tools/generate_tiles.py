#!/usr/bin/env python3
"""Generate tile graphics and palette for KN5000 Mines port.

Produces:
  build/tiles.bin   - 8x8 pixel tiles, 1 byte per pixel (palette index)
  build/palette.bin - 256-entry VGA palette (R,G,B, 6-bit values 0-63)
  build/icon.bin    - 27x27 pixel icon for DISK MENU display
"""

import os
import struct
import sys

# Palette indices (first 16 colors used by game, rest black)
COL_TRANSPARENT = 0   # Black / transparent
COL_DARK_GREEN  = 1   # #007327 - ground, borders
COL_BROWN       = 2   # #522700 - closed cell texture
COL_PINK        = 3   # #FF279B - cursor highlight
COL_BLACK       = 4   # #000000 - outlines, bomb
COL_GREEN       = 5   # #009B52 - ground variation
COL_BLUE        = 6   # #2752FF - number: 1
COL_BRIGHT_GREEN= 7   # #00BA00 - number: 2
COL_RED         = 8   # #FF2727 - number: 3, explosion, bomb
COL_CYAN        = 9   # #00BAFF - number: 4
COL_YELLOW      = 10  # #FFE000 - number: 5, flag
COL_WHITE       = 11  # #FFFFFF - blank cell, highlights
COL_LIGHT_GRAY  = 12  # #BABABA - cell shading
COL_DARK_GRAY   = 13  # #737373 - cell shadow
COL_ORANGE      = 14  # #FF9B27 - number: 6
COL_DARK_BLUE   = 15  # #00007F - number: 7, question mark

# Full 256-color palette (RGB, 8-bit values)
PALETTE_8BIT = [
    (0x00, 0x00, 0x00),  # 0: transparent/black
    (0x00, 0x73, 0x27),  # 1: dark green
    (0x52, 0x27, 0x00),  # 2: brown
    (0xFF, 0x27, 0x9B),  # 3: pink (cursor)
    (0x00, 0x00, 0x00),  # 4: black
    (0x00, 0x9B, 0x52),  # 5: green
    (0x27, 0x52, 0xFF),  # 6: blue
    (0x00, 0xBA, 0x00),  # 7: bright green
    (0xFF, 0x27, 0x27),  # 8: red
    (0x00, 0xBA, 0xFF),  # 9: cyan
    (0xFF, 0xE0, 0x00),  # 10: yellow
    (0xFF, 0xFF, 0xFF),  # 11: white
    (0xBA, 0xBA, 0xBA),  # 12: light gray
    (0x73, 0x73, 0x73),  # 13: dark gray
    (0xFF, 0x9B, 0x27),  # 14: orange
    (0x00, 0x00, 0x7F),  # 15: dark blue
]

# Number colors (1-8)
NUM_COLORS = [COL_BLUE, COL_BRIGHT_GREEN, COL_RED, COL_CYAN,
              COL_YELLOW, COL_ORANGE, COL_DARK_BLUE, COL_DARK_GRAY]

# Tile enumeration (must match codes.h)
CLOSED_CELL = 0
ONE_BOMB = 1
# ... through EIGHT_BOMBS = 8
BLANK = 9
CURSOR = 10
BOMB = 11
FLAG = 12
QUESTION_MARK = 13
EXPLOSION = 14
GROUND = 15
EMPTY_FLAG = 16
BOMB_ICON = 17
HOURGLASS = 18

# Screen borders
CORNER_TOP_LEFT = 19
TOP_BORDER__LEFT = 20
TOP_BORDER__RIGHT = 21
CORNER_TOP_RIGHT = 22
LEFT_BORDER__TOP = 23
RIGHT_BORDER__TOP = 24
LEFT_BORDER__BOTTOM = 25
RIGHT_BORDER__BOTTOM = 26
CORNER_BOTTOM_LEFT = 27
BOTTOM_BORDER__LEFT = 28
BOTTOM_BORDER__RIGHT = 29
CORNER_BOTTOM_RIGHT = 30

# Minefield borders
MF_CORNER_TL = 31
MF_TOP_TEE = 32
MF_HORIZ_TOP = 33
MF_CORNER_TR = 34
MF_LEFT_TEE = 35
MF_CROSS = 36
MF_HORIZ_MID = 37
MF_VERT_MID = 38
MF_RIGHT_TEE = 39
MF_VERT_LEFT = 40
MF_VERT_RIGHT = 41
MF_CORNER_BL = 42
MF_BOTTOM_TEE = 43
MF_HORIZ_BOT = 44
MF_CORNER_BR = 45

# Frame tiles
FRAME_TL = 46
FRAME_TC = 47
FRAME_TR = 48
FRAME_VL = 49
FRAME_VR = 50
FRAME_BL = 51
FRAME_BC = 52
FRAME_BR = 53

# Counter/timer
COLON = 54
NO_SIGN = 55
MINUS_SIGN = 56
ZERO_DIGIT = 57

MAX_VIDEO_TILES = 67  # NINE_DIGIT + 1
NUM_GROUND_VARIANTS = 4
TOTAL_TILES = MAX_VIDEO_TILES + NUM_GROUND_VARIANTS


def make_tile(pixels):
    """Create 8x8 tile from list of 8 strings, each 8 chars of hex palette indices."""
    data = bytearray(64)
    for y in range(8):
        for x in range(8):
            data[y * 8 + x] = int(pixels[y][x], 16) if pixels[y][x] != '.' else 0
    return bytes(data)


def make_number_tile(digit, color):
    """Generate a tile for a number 1-8 on white background."""
    bg = '%x' % COL_WHITE
    fg = '%x' % color

    # 5x7 digit bitmaps (inside 8x8 tile with white border)
    DIGITS_5x7 = {
        1: [".fff.", "..ff.", "...f.", "...f.", "...f.", "...f.", ".ffff"],
        2: [".fff.", "f...f", "....f", "..ff.", ".f...", "f....", "fffff"],
        3: [".fff.", "f...f", "....f", "..ff.", "....f", "f...f", ".fff."],
        4: ["...ff", "..f.f", ".f..f", "f...f", "fffff", "....f", "....f"],
        5: ["fffff", "f....", "ffff.", "....f", "....f", "f...f", ".fff."],
        6: [".fff.", "f....", "ffff.", "f...f", "f...f", "f...f", ".fff."],
        7: ["fffff", "....f", "...f.", "..f..", "..f..", "..f..", "..f.."],
        8: [".fff.", "f...f", "f...f", ".fff.", "f...f", "f...f", ".fff."],
    }

    bmp = DIGITS_5x7[digit]
    rows = [bg * 8]  # top row padding
    for row in bmp:
        r = bg  # left padding
        for c in row:
            r += fg if c == 'f' else bg
        r += bg + bg  # right padding
        rows.append(r)
    return make_tile(rows)


def make_decimal_digit_tile(digit, color=COL_BLACK):
    """Generate a tile for decimal digit 0-9 on white background."""
    bg = '%x' % COL_WHITE
    fg = '%x' % color

    DIGITS_5x7 = {
        0: [".fff.", "f...f", "f..ff", "f.f.f", "ff..f", "f...f", ".fff."],
        1: ["..f..", ".ff..", "..f..", "..f..", "..f..", "..f..", ".fff."],
        2: [".fff.", "f...f", "....f", "..ff.", ".f...", "f....", "fffff"],
        3: [".fff.", "f...f", "....f", "..ff.", "....f", "f...f", ".fff."],
        4: ["...ff", "..f.f", ".f..f", "f...f", "fffff", "....f", "....f"],
        5: ["fffff", "f....", "ffff.", "....f", "....f", "f...f", ".fff."],
        6: [".fff.", "f....", "ffff.", "f...f", "f...f", "f...f", ".fff."],
        7: ["fffff", "....f", "...f.", "..f..", "..f..", "..f..", "..f.."],
        8: [".fff.", "f...f", "f...f", ".fff.", "f...f", "f...f", ".fff."],
        9: [".fff.", "f...f", "f...f", ".ffff", "....f", "....f", ".fff."],
    }

    bmp = DIGITS_5x7[digit]
    rows = [bg * 8]
    for row in bmp:
        r = bg
        for c in row:
            r += fg if c == 'f' else bg
        r += bg + bg
        rows.append(r)
    return make_tile(rows)


# ======== Tile definitions ========

def gen_closed_cell():
    """Raised 3D closed cell."""
    W = COL_WHITE; G = COL_LIGHT_GRAY; D = COL_DARK_GRAY
    t = bytearray(64)
    for y in range(8):
        for x in range(8):
            if y == 0 or x == 0:
                t[y*8+x] = W  # top/left highlight
            elif y == 7 or x == 7:
                t[y*8+x] = D  # bottom/right shadow
            else:
                t[y*8+x] = G  # fill
    return bytes(t)


def gen_blank():
    """Open empty cell (white)."""
    return bytes([COL_WHITE] * 64)


def gen_cursor():
    """Cursor overlay - pink border with transparent center."""
    P = COL_PINK; T = COL_TRANSPARENT
    rows = [
        "33333333",
        "3......3",
        "3......3",
        "3......3",
        "3......3",
        "3......3",
        "3......3",
        "33333333",
    ]
    return make_tile(rows)


def gen_bomb():
    """Bomb icon - black circle with fuse."""
    B = COL_BLACK; W = COL_WHITE; R = COL_RED
    rows = [
        "bb..bb.b",
        "b..bb..b",
        "b.bbbb.b",
        "bbbb4bbb",
        "b.b44b.b",
        "b.bbbb.b",
        "bb.bb.bb",
        "bbb..bbb",
    ]
    # Replace letters with palette indices
    t = bytearray(64)
    for y in range(8):
        for x in range(8):
            c = rows[y][x]
            if c == '4': t[y*8+x] = COL_BLACK
            elif c == 'b': t[y*8+x] = COL_WHITE
            elif c == '.': t[y*8+x] = COL_BLACK
            else: t[y*8+x] = COL_BLACK
    # Redraw properly
    _ = COL_WHITE; B = COL_BLACK
    pixels = [
        [_,_,B,B,B,_,_,_],
        [_,_,_,B,_,B,_,_],
        [_,B,B,B,B,B,_,_],
        [B,B,B,B,B,B,B,_],
        [B,B,_,B,B,B,B,_],
        [B,B,B,B,B,B,B,_],
        [_,B,B,B,B,B,_,_],
        [_,_,B,B,B,_,_,_],
    ]
    t = bytearray(64)
    for y in range(8):
        for x in range(8):
            t[y*8+x] = pixels[y][x]
    return bytes(t)


def gen_flag():
    """Red flag on pole."""
    _ = COL_WHITE; R = COL_RED; B = COL_BLACK
    pixels = [
        [_,_,B,_,_,_,_,_],
        [_,_,B,R,R,_,_,_],
        [_,_,B,R,R,R,_,_],
        [_,_,B,R,R,_,_,_],
        [_,_,B,_,_,_,_,_],
        [_,_,B,_,_,_,_,_],
        [_,B,B,B,_,_,_,_],
        [B,B,B,B,B,_,_,_],
    ]
    t = bytearray(64)
    for y in range(8):
        for x in range(8):
            t[y*8+x] = pixels[y][x]
    return bytes(t)


def gen_question():
    """Question mark on closed cell."""
    _ = COL_LIGHT_GRAY; Q = COL_DARK_BLUE; W = COL_WHITE; D = COL_DARK_GRAY
    pixels = [
        [W,W,W,W,W,W,W,D],
        [W,_,Q,Q,Q,_,_,D],
        [W,_,_,_,_,Q,_,D],
        [W,_,_,_,Q,_,_,D],
        [W,_,_,Q,_,_,_,D],
        [W,_,_,_,_,_,_,D],
        [W,_,_,Q,_,_,_,D],
        [D,D,D,D,D,D,D,D],
    ]
    t = bytearray(64)
    for y in range(8):
        for x in range(8):
            t[y*8+x] = pixels[y][x]
    return bytes(t)


def gen_explosion():
    """Explosion - red/yellow burst."""
    _ = COL_WHITE; R = COL_RED; Y = COL_YELLOW; O = COL_ORANGE
    pixels = [
        [R,_,_,R,_,_,R,_],
        [_,R,Y,O,Y,R,_,_],
        [_,Y,O,Y,O,Y,_,_],
        [R,O,Y,R,Y,O,R,_],
        [_,Y,O,Y,O,Y,_,_],
        [_,R,Y,O,Y,R,_,_],
        [R,_,_,R,_,_,R,_],
        [_,_,R,_,R,_,_,_],
    ]
    t = bytearray(64)
    for y in range(8):
        for x in range(8):
            t[y*8+x] = pixels[y][x]
    return bytes(t)


def gen_ground(variant=0):
    """Ground tile with subtle texture."""
    G1 = COL_DARK_GREEN; G2 = COL_GREEN
    if variant == 0:
        return bytes([G1] * 64)
    elif variant == 1:
        t = bytearray([G1] * 64)
        t[3*8+5] = G2; t[6*8+2] = G2
        return bytes(t)
    elif variant == 2:
        t = bytearray([G1] * 64)
        t[1*8+3] = G2; t[5*8+6] = G2
        return bytes(t)
    else:
        t = bytearray([G1] * 64)
        t[2*8+1] = G2; t[4*8+4] = G2; t[7*8+6] = G2
        return bytes(t)


def gen_border_tile(top, bottom, left, right, fill=COL_DARK_GREEN):
    """Generic border tile with colored edges."""
    t = bytearray([fill] * 64)
    B = COL_BROWN
    if top:
        for x in range(8): t[0*8+x] = B; t[1*8+x] = B
    if bottom:
        for x in range(8): t[6*8+x] = B; t[7*8+x] = B
    if left:
        for y in range(8): t[y*8+0] = B; t[y*8+1] = B
    if right:
        for y in range(8): t[y*8+6] = B; t[y*8+7] = B
    return bytes(t)


def gen_minefield_border(directions):
    """Minefield grid border.
    directions: string of N/S/E/W indicating which directions have lines."""
    _ = COL_WHITE; L = COL_DARK_GRAY
    t = bytearray([_] * 64)

    has_n = 'N' in directions
    has_s = 'S' in directions
    has_e = 'E' in directions
    has_w = 'W' in directions

    # Horizontal line through middle
    if has_w or has_e:
        for x in range(8):
            if has_w and has_e:
                t[3*8+x] = L; t[4*8+x] = L
            elif has_w:
                if x < 5: t[3*8+x] = L; t[4*8+x] = L
            elif has_e:
                if x > 2: t[3*8+x] = L; t[4*8+x] = L

    # Vertical line through middle
    if has_n or has_s:
        for y in range(8):
            if has_n and has_s:
                t[y*8+3] = L; t[y*8+4] = L
            elif has_n:
                if y < 5: t[y*8+3] = L; t[y*8+4] = L
            elif has_s:
                if y > 2: t[y*8+3] = L; t[y*8+4] = L

    return bytes(t)


def gen_frame_tile(has_top, has_bottom, has_left, has_right, fill=COL_WHITE):
    """Frame tile for counter/timer display."""
    t = bytearray([fill] * 64)
    L = COL_DARK_GRAY
    if has_top:
        for x in range(8): t[0*8+x] = L
    if has_bottom:
        for x in range(8): t[7*8+x] = L
    if has_left:
        for y in range(8): t[y*8+0] = L
    if has_right:
        for y in range(8): t[y*8+7] = L
    return bytes(t)


def gen_colon():
    _ = COL_WHITE; D = COL_BLACK
    pixels = [
        [_,_,_,_,_,_,_,_],
        [_,_,_,_,_,_,_,_],
        [_,_,_,D,D,_,_,_],
        [_,_,_,_,_,_,_,_],
        [_,_,_,_,_,_,_,_],
        [_,_,_,D,D,_,_,_],
        [_,_,_,_,_,_,_,_],
        [_,_,_,_,_,_,_,_],
    ]
    t = bytearray(64)
    for y in range(8):
        for x in range(8):
            t[y*8+x] = pixels[y][x]
    return bytes(t)


def gen_no_sign():
    """Empty/no sign tile."""
    return bytes([COL_WHITE] * 64)


def gen_minus():
    _ = COL_WHITE; D = COL_BLACK
    pixels = [
        [_,_,_,_,_,_,_,_],
        [_,_,_,_,_,_,_,_],
        [_,_,_,_,_,_,_,_],
        [_,D,D,D,D,D,_,_],
        [_,_,_,_,_,_,_,_],
        [_,_,_,_,_,_,_,_],
        [_,_,_,_,_,_,_,_],
        [_,_,_,_,_,_,_,_],
    ]
    t = bytearray(64)
    for y in range(8):
        for x in range(8):
            t[y*8+x] = pixels[y][x]
    return bytes(t)


def gen_empty_flag():
    """Empty flag (wrong flag placement, shown at game over)."""
    _ = COL_WHITE; B = COL_BLACK; X = COL_RED
    pixels = [
        [_,_,B,_,_,_,_,_],
        [_,_,B,_,_,_,_,_],
        [_,_,B,_,_,_,_,_],
        [_,_,B,_,_,_,_,_],
        [_,_,B,_,_,_,_,_],
        [X,_,B,_,X,_,_,_],
        [_,X,B,X,_,_,_,_],
        [_,_,X,_,_,_,_,_],
    ]
    t = bytearray(64)
    for y in range(8):
        for x in range(8):
            t[y*8+x] = pixels[y][x]
    return bytes(t)


def gen_bomb_icon():
    """Small bomb icon for counter display."""
    return gen_bomb()


def gen_hourglass():
    """Hourglass icon for timer display."""
    _ = COL_WHITE; B = COL_BLACK; Y = COL_YELLOW
    pixels = [
        [B,B,B,B,B,B,_,_],
        [_,B,Y,Y,B,_,_,_],
        [_,_,B,B,_,_,_,_],
        [_,_,B,B,_,_,_,_],
        [_,_,B,B,_,_,_,_],
        [_,B,Y,Y,B,_,_,_],
        [B,B,B,B,B,B,_,_],
        [_,_,_,_,_,_,_,_],
    ]
    t = bytearray(64)
    for y in range(8):
        for x in range(8):
            t[y*8+x] = pixels[y][x]
    return bytes(t)


def generate_icon():
    """Generate 27x27 pixel icon for DISK MENU.
    Shows a simplified minesweeper grid with a bomb."""
    G = COL_DARK_GREEN; W = COL_WHITE; B = COL_BLACK
    D = COL_DARK_GRAY; R = COL_RED; L = COL_LIGHT_GRAY

    icon = bytearray(27 * 27)
    # Fill with green background
    for i in range(27 * 27):
        icon[i] = G

    # Draw a 3x3 grid of cells in the center
    for gy in range(3):
        for gx in range(3):
            cx = 4 + gx * 7
            cy = 4 + gy * 7
            # Draw cell background
            for dy in range(6):
                for dx in range(6):
                    icon[(cy+dy)*27 + cx+dx] = L
            # Highlight edges
            for dx in range(6):
                icon[cy*27 + cx+dx] = W  # top
            for dy in range(6):
                icon[(cy+dy)*27 + cx] = W  # left
            for dx in range(6):
                icon[(cy+5)*27 + cx+dx] = D  # bottom
            for dy in range(6):
                icon[(cy+dy)*27 + cx+5] = D  # right

    # Draw a bomb in center cell
    cx, cy = 4 + 7, 4 + 7  # center cell
    bomb_pixels = [
        (1,0),(2,0),(3,0),
        (0,1),(1,1),(2,1),(3,1),(4,1),
        (0,2),(1,2),(2,2),(3,2),(4,2),
        (0,3),(1,3),(2,3),(3,3),(4,3),
        (1,4),(2,4),(3,4),
    ]
    for dx, dy in bomb_pixels:
        icon[(cy+dy)*27 + cx+dx] = B

    # Draw a flag in top-left cell
    cx, cy = 4, 4
    flag_pixels = [(2,0,B),(2,1,R),(3,1,R),(2,2,R),(3,2,R),(4,2,R),
                   (2,3,R),(3,3,R),(2,4,B),(1,5,B),(2,5,B),(3,5,B)]
    for dx, dy, c in flag_pixels:
        icon[(cy+dy)*27 + cx+dx] = c

    return bytes(icon)


def main():
    if len(sys.argv) < 2:
        outdir = "build"
    else:
        outdir = sys.argv[1]

    os.makedirs(outdir, exist_ok=True)

    tiles = bytearray()

    # Generate all tiles in order matching codes.h enum

    # 0: CLOSED_CELL
    tiles += gen_closed_cell()

    # 1-8: ONE_BOMB through EIGHT_BOMBS
    for i in range(1, 9):
        tiles += make_number_tile(i, NUM_COLORS[i-1])

    # 9: BLANK
    tiles += gen_blank()

    # 10: CURSOR
    tiles += gen_cursor()

    # 11: BOMB
    tiles += gen_bomb()

    # 12: FLAG
    tiles += gen_flag()

    # 13: QUESTION_MARK
    tiles += gen_question()

    # 14: EXPLOSION
    tiles += gen_explosion()

    # 15: GROUND (default variant - used as lookup, variants are appended later)
    tiles += gen_ground(0)

    # 16: EMPTY_FLAG
    tiles += gen_empty_flag()

    # 17: BOMB_ICON
    tiles += gen_bomb_icon()

    # 18: HOURGLASS
    tiles += gen_hourglass()

    # 19-30: Screen borders
    # CORNER_TOP_LEFT, TOP_BORDER__LEFT, TOP_BORDER__RIGHT, CORNER_TOP_RIGHT
    tiles += gen_border_tile(top=True, bottom=False, left=True, right=False)
    tiles += gen_border_tile(top=True, bottom=False, left=False, right=False)
    tiles += gen_border_tile(top=True, bottom=False, left=False, right=False)
    tiles += gen_border_tile(top=True, bottom=False, left=False, right=True)
    # LEFT_BORDER__TOP, RIGHT_BORDER__TOP
    tiles += gen_border_tile(top=False, bottom=False, left=True, right=False)
    tiles += gen_border_tile(top=False, bottom=False, left=False, right=True)
    # LEFT_BORDER__BOTTOM, RIGHT_BORDER__BOTTOM
    tiles += gen_border_tile(top=False, bottom=False, left=True, right=False)
    tiles += gen_border_tile(top=False, bottom=False, left=False, right=True)
    # CORNER_BOTTOM_LEFT, BOTTOM_BORDER__LEFT, BOTTOM_BORDER__RIGHT, CORNER_BOTTOM_RIGHT
    tiles += gen_border_tile(top=False, bottom=True, left=True, right=False)
    tiles += gen_border_tile(top=False, bottom=True, left=False, right=False)
    tiles += gen_border_tile(top=False, bottom=True, left=False, right=False)
    tiles += gen_border_tile(top=False, bottom=True, left=False, right=True)

    # 31-45: Minefield borders
    tiles += gen_minefield_border('SE')    # CORNER_TOP_LEFT
    tiles += gen_minefield_border('SEW')   # TOP_TEE
    tiles += gen_minefield_border('EW')    # HORIZONTAL_TOP
    tiles += gen_minefield_border('SW')    # CORNER_TOP_RIGHT
    tiles += gen_minefield_border('NSE')   # LEFT_TEE
    tiles += gen_minefield_border('NSEW')  # CROSS
    tiles += gen_minefield_border('EW')    # HORIZONTAL_MIDDLE
    tiles += gen_minefield_border('NS')    # VERTICAL_MIDDLE
    tiles += gen_minefield_border('NSW')   # RIGHT_TEE
    tiles += gen_minefield_border('NS')    # VERTICAL_LEFT
    tiles += gen_minefield_border('NS')    # VERTICAL_RIGHT
    tiles += gen_minefield_border('NE')    # CORNER_BOTTOM_LEFT
    tiles += gen_minefield_border('NEW')   # BOTTOM_TEE
    tiles += gen_minefield_border('EW')    # HORIZONTAL_BOTTOM
    tiles += gen_minefield_border('NW')    # CORNER_BOTTOM_RIGHT

    # 46-53: Frame tiles
    tiles += gen_frame_tile(True, False, True, False)    # FRAME_TOP_LEFT
    tiles += gen_frame_tile(True, False, False, False)   # FRAME_TOP_CENTER
    tiles += gen_frame_tile(True, False, False, True)    # FRAME_TOP_RIGHT
    tiles += gen_frame_tile(False, False, True, False)   # FRAME_VERTICAL_LEFT
    tiles += gen_frame_tile(False, False, False, True)   # FRAME_VERTICAL_RIGHT
    tiles += gen_frame_tile(False, True, True, False)    # FRAME_BOTTOM_LEFT
    tiles += gen_frame_tile(False, True, False, False)   # FRAME_BOTTOM_CENTER
    tiles += gen_frame_tile(False, True, False, True)    # FRAME_BOTTOM_RIGHT

    # 54: COLON
    tiles += gen_colon()

    # 55: NO_SIGN
    tiles += gen_no_sign()

    # 56: MINUS_SIGN
    tiles += gen_minus()

    # 57-66: ZERO_DIGIT through NINE_DIGIT
    for d in range(10):
        tiles += make_decimal_digit_tile(d)

    # Verify we have MAX_VIDEO_TILES tiles
    assert len(tiles) == MAX_VIDEO_TILES * 64, \
        f"Expected {MAX_VIDEO_TILES * 64} bytes, got {len(tiles)}"

    # Append ground variants (4 variants, referenced after MAX_VIDEO_TILES)
    for v in range(NUM_GROUND_VARIANTS):
        tiles += gen_ground(v)

    # Write tiles.bin
    tiles_path = os.path.join(outdir, "tiles.bin")
    with open(tiles_path, 'wb') as f:
        f.write(tiles)
    print(f"Wrote {len(tiles)} bytes to {tiles_path} "
          f"({TOTAL_TILES} tiles, {TOTAL_TILES * 64} bytes)")

    # Generate palette (256 entries * 3 bytes, 6-bit VGA values)
    palette = bytearray(256 * 3)
    for i, (r, g, b) in enumerate(PALETTE_8BIT):
        # Convert 8-bit RGB to 6-bit VGA DAC values (0-63)
        palette[i*3 + 0] = r >> 2
        palette[i*3 + 1] = g >> 2
        palette[i*3 + 2] = b >> 2
    # Rest of palette entries remain 0 (black)

    palette_path = os.path.join(outdir, "palette.bin")
    with open(palette_path, 'wb') as f:
        f.write(palette)
    print(f"Wrote {len(palette)} bytes to {palette_path} (256 palette entries)")

    # Generate icon
    icon = generate_icon()
    icon_path = os.path.join(outdir, "icon.bin")
    with open(icon_path, 'wb') as f:
        f.write(icon)
    print(f"Wrote {len(icon)} bytes to {icon_path} (27x27 icon)")


if __name__ == '__main__':
    main()
