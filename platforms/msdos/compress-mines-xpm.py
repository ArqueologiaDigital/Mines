#!/usr/bin/python
# Crude XPM loader (generated by GIMP)/RLE compressor

import struct
import itertools
import sys

STATE_BEGIN = 0
STATE_READ_ARRAY = 1
STATE_READ_HEADER = 2
STATE_READ_PALETTE = 3
STATE_READ_BITMAP = 4

state = STATE_BEGIN
width = 0
height = 0
num_colors = 0
bpc = 0

palette = {}
bitmap= []

with open("mines.xpm", "r") as mines:
  for line in mines:
    line = line.strip()
    
    if state == STATE_BEGIN:
      if line == "/* XPM */":
        state = STATE_READ_ARRAY
        continue
      raise SyntaxError("XPM doesn't start with /* XPM */")

    if state == STATE_READ_ARRAY:
      if line.startswith("static char *"):
        state = STATE_READ_HEADER
        continue
      raise SyntaxError("Could not find array definition")
    
    if state == STATE_READ_HEADER:
      width, height, num_colors, bpc = line[1:-2].split()
      width = int(width)
      height = int(height)
      num_colors = int(num_colors)
      bpc = int(bpc)

      if bpc != 1:
        raise SyntaxError(f"Don't know how to read XPM with {bpc} bytes per char")

      if num_colors > 15:
        raise SyntaxError("Only know how to read XPMs with at most 15 colors")

      state = STATE_READ_PALETTE
      continue

    if state == STATE_READ_PALETTE:
      line = line[1:-2]
      character = line[0]
      c, hex = line[1:].strip().split(" ")
      if c != "c":
        raise SyntaxError("Expecting a color here")

      r = int(hex[1:3], 16)
      g = int(hex[3:5], 16)
      b = int(hex[5:7], 16)

      palette[character] = (r, g, b)
      if len(palette) == num_colors:
        state = STATE_READ_BITMAP
      continue
  
    if state == STATE_READ_BITMAP:
      if line.endswith("};"):
        bitmap.append(line[1:-3])
      else:
        bitmap.append(line[1:-2])
      if len(bitmap) == height:
        break

def compress(lines):
  for line in bitmap:
    encoded = ((len(tuple(group)), c) for c, group in itertools.groupby(line))
    for reps, c in encoded:
      c = char_to_index[c]
      while reps:
        cur_reps = min(18, reps)
        if cur_reps < 4:
          for _ in range(cur_reps):
            yield c
        else:
          yield 0xf
          yield cur_reps - 3
          yield c
        reps -= cur_reps

with open(sys.argv[1], "wb") as mines:
  mines.write(struct.pack("HHB", width, height, num_colors))
  for r, g, b in palette.values():
    r = int((r * 7.0) // 0xff) << 3
    g = int((g * 7.0) // 0xff) << 3
    b = int((b * 7.0) // 0xff) << 3
    mines.write(struct.pack("BBB", r, g, b))

  char_to_index = {"\xff": 0xF}
  for index, c in enumerate(palette.keys()):
    char_to_index[c] = index

  b = 0
  for index, nibble in enumerate(compress(bitmap)):
    b = ((b << 4) | nibble) & 0xff
    if index & 1:
      mines.write(struct.pack("B", b))
      
