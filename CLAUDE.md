# Mines - Minesweeper for Multiple Platforms

## Project Overview

Cross-platform minesweeper game with platform-specific backends. The KN5000 port targets the Technics KN5000 music keyboard via its HDAE5000 extension board slot.

## Build

### KN5000 Platform
```bash
cd platforms/kn5000
make          # Build ROM + install to MAME ROM set
make test     # Run in MAME (needs Qt5 display)
```

Requires custom LLVM with TLCS-900 backend at `../llvm-project/build/bin/`.

## Architecture

### Common Code (`common/`)
- `main.c` - Game loop: platform_init() -> init_minefield() -> draw_scenario() -> gameplay loop
- `minefield.c` - Game logic (mine placement, flood fill, win/lose detection)
- `tiles.c` - Tile rendering dispatch (calls platform set_tile())

### KN5000 Platform (`platforms/kn5000/`)
- `startup.s` - Assembly: XAPR header, Boot_Init, Frame_Handler, HANDLER_REGISTRATION, C runtime init, compiler helpers (__udivsi3, __umodsi3)
- `video.c` - Display driver: VGA palette loading, tile-based rendering to 320x240 8bpp framebuffer
- `input.c` - Control panel input via SC1 synchronous serial
- `extras.c` - rand()/srand() implementation
- `kn5000.h` - Hardware definitions (memory map, VGA registers, SFR addresses, button mappings)
- `kn5000.ld` - Linker script (ROM at 0x280000, RAM at 0x200000)
- `tools/generate_tiles.py` - Generates tile graphics and palette data

## KN5000 Extension Board Interface

### Current Status
- DISK MENU entry with mine icon and "Mines Game" text: **WORKING**
- Game activation from DISK MENU selection: **NOT IMPLEMENTED**
- Display ownership (drawing to LCD): **NOT IMPLEMENTED**
- Control panel input in game: **NEEDS FIRMWARE API**

### Display Ownership (Critical Blocker)
The firmware draws its UI in the main event loop BEFORE calling Frame_Handler. To render game graphics:

1. Set bit 3 of SFR address `0x0D53` to disable firmware drawing
2. Load custom VGA palette (4-bit RAMDAC, shift 6-bit values >> 2)
3. Write directly to VRAM at 0x1A0000 (320x240 linear framebuffer)

The MN89304 VGA controller uses a 4-bit RAMDAC (not standard 6-bit VGA). Key registers:
- DAC write index: 0x1703C8
- DAC data: 0x1703C9 (write R,G,B sequentially)
- CRTC index/data: 0x1703D4/0x1703D5

### DISK MENU Activation (Next Step)
When user selects our DISK MENU entry, firmware queries handler `0x01600040`. We need to register a sub-handler via `workspace[0x0E0A][0x00E4]` to respond. The original HDAE5000 uses handler ID `0x016A` with data at `0x29C0AA`.

### Workspace Pointer System
Boot_Init receives workspace pointer `0x027ED2` in XWA. Key offsets:
- `workspace[+0x0E0A]` -> Handler Table A (registration, callbacks, display)
- `workspace[+0x0E88]` -> Handler Table B (init functions)
- Table A `[+0x02C4]` -> DISK MENU registration function
- Table A `[+0x00E4]` -> Handler registration function
- Table A `[+0x0124]` -> Display callback
- Table A `[+0x0244/0x0248/0x024C]` -> UI callbacks

### LLVM Assembler Limitations
All encoding bugs are fixed. Remaining unsupported features:
- `ei` / `di` instructions: use raw `.byte` opcodes
- Large displacement loads (>127 bytes): use `add + ld (xreg)` workaround
- `ld a, (addr)` direct byte load: use register-indirect

## Research Documentation
- Extension board interface: `~/devel/kn5000-docs/hdae5000-homebrew.md`
- Display subsystem: `~/devel/kn5000-docs/display-subsystem.md`
- Original HDAE5000 firmware: `~/devel/kn5000-docs/hdae5000.md`
- ROM disassembly: `~/devel/kn5000-roms-disasm/`

## Next Steps (in order)
1. Understand DISK MENU activation mechanism (handler 0x016A protocol)
2. Implement display ownership (0x0D53 bit 3 + palette + VRAM)
3. Implement firmware-mediated input (workspace UI callbacks)
4. Wire up game activation flow (DISK MENU -> Activate_Game -> main())
