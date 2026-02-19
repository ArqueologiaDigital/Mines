# Mines for Technics KN5000

Port of the Mines minesweeper game to the Technics KN5000 music keyboard, packaged as a 512KB HDAE5000 extension ROM.

## Hardware

- **CPU**: TMP94C241F (TLCS-900/H2, 32-bit CISC, 16 MHz)
- **Display**: 320x240 LCD, 8bpp indexed color
- **Input**: Control panel buttons via SC1 serial protocol

## Building

### Requirements

- LLVM TLCS900 compiler (built from `../llvm-project/`)
- ASL Macro Assembler (`asl` and `p2bin`)
- Python 3 (for tile generation)
- Original KN5000 ROMs (for MAME testing)

### Build

```
make          # Build ROM and create MAME ROM set
make build    # Build ROM only
make test     # Run in MAME emulator
make check    # Verify build tools are available
```

### Output

- `build/mines_kn5000.bin` — 512KB extension ROM
- MAME ROM set at `/mnt/shared/custom_kn5000_roms/mines/kn5000/`

## Button Mapping

| Action      | KN5000 Button         | Panel   |
|-------------|----------------------|---------|
| Move Up     | Part Select: Right 2 | Right   |
| Move Down   | Conductor: Right 2   | Right   |
| Move Left   | Conductor: Left      | Right   |
| Move Right  | Conductor: Right 1   | Right   |
| Open Cell   | Variation 4          | Left    |
| Toggle Flag | Page Up              | Left    |
| Quit        | Exit                 | Left    |

## How It Works

The game replaces the HDAE5000 hard disk expansion firmware. When the KN5000 boots with this ROM installed:

1. The main firmware detects the "HDAE5000" board via PE port bit 0
2. Our Boot Init registers handlers with the firmware's callback system
3. When the user opens the DISK MENU, they see a "Mines" icon
4. Selecting it activates the game: loads the game palette, clears the screen, and runs the minesweeper
5. Pressing Exit returns to normal keyboard operation

## Architecture

```
C sources (video.c, input.c, extras.c)
  + common sources (main.c, minefield.c, tiles.c)
    → clang --target=tlcs900 → LLVM IR
      → llc → ELF objects
        → llvm-objcopy → raw binary

main.asm (ASL assembly)
  + hardware init includes
  + handler registration
  + incbin compiled C code
  + incbin tile/palette data
    → ASL → p2bin → 512KB extension ROM
```

## ROM Layout

| Address Range       | Content                |
|--------------------|------------------------|
| 0x280000-0x280007  | XAPR header            |
| 0x280008-0x28000F  | Entry point jumps      |
| 0x280020-0x2803FF  | Handler registration   |
| 0x280400-0x280FFF  | Assembly glue code     |
| 0x281000-0x2Exxxx  | Compiled C code        |
| 0x2Exxxx-0x2FFFFF  | Tile/palette/icon data |
