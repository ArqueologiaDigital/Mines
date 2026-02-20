#!/usr/bin/env python3
"""patch_icon.py - Patch a custom mine icon into KN5000 table_data ROMs.

Creates a 24x24 4bpp mine icon and patches it as icon ID 176 into the
table_data ROM files (even/odd interleaved).

Icon table format:
  - Entry: 2B width + 2B height + 4B data_ptr (little-endian)
  - Pixel data: 4bpp, high nibble = left pixel, low nibble = right pixel
  - 12 bytes/row, 24 rows = 288 bytes per icon

ROM interleaving (ROM_LOAD32_WORD):
  CPU bytes [0,1] from even ROM (ic3), CPU bytes [2,3] from odd ROM (ic1)

Usage: python3 patch_icon.py <romset_dir>
  Reads ic3/ic1 from romset_dir, patches in place.
"""

import sys
import os
import struct

# --- Constants ---
ICON_ID = 176
ICON_W = 24
ICON_H = 24
BYTES_PER_ROW = 12   # 24 pixels / 2 pixels per byte
ICON_DATA_SIZE = 288  # 24 * 12

# Region offset of icon table (CPU 0x938000 - region base 0x800000)
ICON_TABLE_OFFSET = 0x138000

# Null sentinel is at entry 176 (right after the last entry)
NEW_ENTRY_OFFSET = ICON_TABLE_OFFSET + ICON_ID * 8  # 0x138580

# Free space for pixel data starts at region 0x144C6C
NEW_DATA_REGION_OFFSET = 0x144C6C
NEW_DATA_CPU_ADDR = 0x800000 + NEW_DATA_REGION_OFFSET  # 0x944C6C

# ROM filenames
EVEN_ROM = "kn5000_table_data_rom_even.ic3"
ODD_ROM = "kn5000_table_data_rom_odd.ic1"

# --- Mine icon pixel art (24x24, palette indices) ---
# Icon palette (CGA-like, mapped through nibble lookup table):
#   0x0 = Black (#000000)   - outlines, mine body
#   0x7 = LtGray (#888888)  - icon background
#   0x8 = DkGray (#606060)  - mine body shading
#   0x9 = Red (#FF0000)     - danger accent
#   0xF = White (#FFFFFF)   - highlight/shine
BG = 0x7  # light gray background
BK = 0x0  # black (mine body + spikes)
SH = 0x8  # dark gray (body shading)
RD = 0x9  # red (danger accent on spikes)
WH = 0xF  # white (highlight)

_B = BG
_K = BK
_S = SH
_R = RD
_W = WH

MINE_ICON = [
    # Row 0
    [_B,_B,_B,_B,_B,_B,_B,_B,_B,_B,_B,_B,_B,_B,_B,_B,_B,_B,_B,_B,_B,_B,_B,_B],
    # Row 1: top spike
    [_B,_B,_B,_B,_B,_B,_B,_B,_B,_B,_B,_R,_R,_B,_B,_B,_B,_B,_B,_B,_B,_B,_B,_B],
    # Row 2
    [_B,_B,_B,_B,_B,_B,_B,_B,_B,_B,_B,_K,_K,_B,_B,_B,_B,_B,_B,_B,_B,_B,_B,_B],
    # Row 3: diagonal spikes
    [_B,_B,_B,_R,_B,_B,_B,_B,_B,_B,_B,_K,_K,_B,_B,_B,_B,_B,_B,_B,_R,_B,_B,_B],
    # Row 4
    [_B,_B,_B,_B,_K,_B,_B,_B,_B,_B,_B,_K,_K,_B,_B,_B,_B,_B,_B,_K,_B,_B,_B,_B],
    # Row 5: body starts
    [_B,_B,_B,_B,_B,_K,_B,_B,_K,_K,_K,_K,_K,_K,_K,_K,_B,_B,_K,_B,_B,_B,_B,_B],
    # Row 6
    [_B,_B,_B,_B,_B,_B,_K,_K,_K,_K,_K,_K,_K,_K,_K,_K,_K,_K,_B,_B,_B,_B,_B,_B],
    # Row 7
    [_B,_B,_B,_B,_B,_B,_K,_K,_K,_K,_K,_K,_K,_K,_K,_K,_K,_K,_B,_B,_B,_B,_B,_B],
    # Row 8: side spikes
    [_B,_B,_R,_K,_B,_K,_K,_K,_K,_W,_W,_K,_K,_K,_K,_K,_K,_K,_K,_B,_K,_R,_B,_B],
    # Row 9
    [_B,_B,_B,_B,_K,_K,_K,_K,_W,_W,_K,_K,_K,_K,_K,_K,_K,_K,_K,_K,_B,_B,_B,_B],
    # Row 10: highlight
    [_B,_B,_B,_B,_K,_K,_K,_K,_W,_K,_K,_K,_K,_K,_K,_S,_S,_K,_K,_K,_B,_B,_B,_B],
    # Row 11: center
    [_B,_B,_R,_K,_K,_K,_K,_K,_K,_K,_K,_K,_K,_K,_S,_S,_S,_K,_K,_K,_K,_R,_B,_B],
    # Row 12: center + side spikes
    [_B,_B,_R,_K,_K,_K,_K,_K,_K,_K,_K,_K,_K,_K,_S,_S,_K,_K,_K,_K,_K,_R,_B,_B],
    # Row 13
    [_B,_B,_B,_B,_K,_K,_K,_K,_K,_K,_K,_K,_K,_K,_K,_S,_K,_K,_K,_K,_B,_B,_B,_B],
    # Row 14
    [_B,_B,_B,_B,_K,_K,_K,_K,_K,_K,_K,_K,_K,_K,_K,_K,_K,_K,_K,_K,_B,_B,_B,_B],
    # Row 15: side spikes
    [_B,_B,_R,_K,_B,_K,_K,_K,_K,_K,_K,_K,_K,_K,_K,_K,_K,_K,_K,_B,_K,_R,_B,_B],
    # Row 16
    [_B,_B,_B,_B,_B,_B,_K,_K,_K,_K,_K,_K,_K,_K,_K,_K,_K,_K,_B,_B,_B,_B,_B,_B],
    # Row 17
    [_B,_B,_B,_B,_B,_B,_K,_K,_K,_K,_K,_K,_K,_K,_K,_K,_K,_K,_B,_B,_B,_B,_B,_B],
    # Row 18: body ends
    [_B,_B,_B,_B,_B,_K,_B,_B,_K,_K,_K,_K,_K,_K,_K,_K,_B,_B,_K,_B,_B,_B,_B,_B],
    # Row 19: diagonal spikes
    [_B,_B,_B,_B,_K,_B,_B,_B,_B,_B,_B,_K,_K,_B,_B,_B,_B,_B,_B,_K,_B,_B,_B,_B],
    # Row 20
    [_B,_B,_B,_R,_B,_B,_B,_B,_B,_B,_B,_K,_K,_B,_B,_B,_B,_B,_B,_B,_R,_B,_B,_B],
    # Row 21: bottom spike
    [_B,_B,_B,_B,_B,_B,_B,_B,_B,_B,_B,_K,_K,_B,_B,_B,_B,_B,_B,_B,_B,_B,_B,_B],
    # Row 22
    [_B,_B,_B,_B,_B,_B,_B,_B,_B,_B,_B,_R,_R,_B,_B,_B,_B,_B,_B,_B,_B,_B,_B,_B],
    # Row 23
    [_B,_B,_B,_B,_B,_B,_B,_B,_B,_B,_B,_B,_B,_B,_B,_B,_B,_B,_B,_B,_B,_B,_B,_B],
]


def pixels_to_4bpp(rows):
    """Convert 24x24 pixel array to 288-byte 4bpp buffer."""
    data = bytearray()
    for row in rows:
        assert len(row) == 24
        for i in range(0, 24, 2):
            byte = (row[i] << 4) | (row[i + 1] & 0x0F)
            data.append(byte)
    assert len(data) == ICON_DATA_SIZE
    return data


def region_to_rom_offsets(region_offset):
    """Convert a region offset to (ic3_offset, ic1_offset) pairs.

    ROM_LOAD32_WORD interleaving:
      CPU[n*4 + 0] = ic3[n*2 + 0]
      CPU[n*4 + 1] = ic3[n*2 + 1]
      CPU[n*4 + 2] = ic1[n*2 + 0]
      CPU[n*4 + 3] = ic1[n*2 + 1]

    So for region_offset:
      word_index = region_offset // 4
      byte_within_word = region_offset % 4
      If byte_within_word < 2: ic3 at word_index*2 + byte_within_word
      If byte_within_word >= 2: ic1 at word_index*2 + (byte_within_word - 2)
    """
    word_idx = region_offset // 4
    byte_in_word = region_offset % 4
    if byte_in_word < 2:
        return ("ic3", word_idx * 2 + byte_in_word)
    else:
        return ("ic1", word_idx * 2 + (byte_in_word - 2))


def write_region_bytes(ic3_data, ic1_data, region_offset, data_bytes):
    """Write bytes to the correct ROM files at the given region offset."""
    for i, b in enumerate(data_bytes):
        rom_file, rom_offset = region_to_rom_offsets(region_offset + i)
        if rom_file == "ic3":
            ic3_data[rom_offset] = b
        else:
            ic1_data[rom_offset] = b


def main():
    if len(sys.argv) != 2:
        print(f"Usage: {sys.argv[0]} <romset_dir>")
        sys.exit(1)

    romset_dir = sys.argv[1]
    ic3_path = os.path.join(romset_dir, EVEN_ROM)
    ic1_path = os.path.join(romset_dir, ODD_ROM)

    # Read ROM files
    with open(ic3_path, "rb") as f:
        ic3_data = bytearray(f.read())
    with open(ic1_path, "rb") as f:
        ic1_data = bytearray(f.read())

    print(f"Read {EVEN_ROM}: {len(ic3_data)} bytes")
    print(f"Read {ODD_ROM}: {len(ic1_data)} bytes")

    # Build icon table entry (8 bytes, little-endian)
    entry = struct.pack("<HHI", ICON_W, ICON_H, NEW_DATA_CPU_ADDR)
    print(f"Icon table entry: {entry.hex()} at region 0x{NEW_ENTRY_OFFSET:06X}")
    print(f"  Width={ICON_W}, Height={ICON_H}, DataPtr=0x{NEW_DATA_CPU_ADDR:08X}")

    # Write table entry
    write_region_bytes(ic3_data, ic1_data, NEW_ENTRY_OFFSET, entry)

    # Add new null sentinel after our entry (8 zero bytes)
    sentinel_offset = NEW_ENTRY_OFFSET + 8
    write_region_bytes(ic3_data, ic1_data, sentinel_offset, b'\x00' * 8)

    # Generate icon pixel data
    icon_data = pixels_to_4bpp(MINE_ICON)
    print(f"Icon pixel data: {len(icon_data)} bytes at region 0x{NEW_DATA_REGION_OFFSET:06X}")

    # Write pixel data
    write_region_bytes(ic3_data, ic1_data, NEW_DATA_REGION_OFFSET, icon_data)

    # Write modified ROM files back
    with open(ic3_path, "wb") as f:
        f.write(ic3_data)
    with open(ic1_path, "wb") as f:
        f.write(ic1_data)

    print(f"Patched {EVEN_ROM} and {ODD_ROM}")
    print(f"New icon ID: {ICON_ID}")

    # Print ASCII preview
    print("\nIcon preview:")
    palette_chars = {0x0: '#', 0x7: '-', 0x8: '%', 0x9: '*', 0xF: '.'}
    for row in MINE_ICON:
        line = ''.join(palette_chars.get(p, '?') for p in row)
        print(f"  |{line}|")


if __name__ == "__main__":
    main()
