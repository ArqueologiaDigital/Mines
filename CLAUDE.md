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
- Game activation from DISK MENU selection: **WORKING** (RegisterObjectTable + Mines_Handler)
- Display/rendering to LCD: **WORKING** (minesweeper board renders correctly)
- Palette loading: **WORKING** (game colors visible)
- Control panel input in game: **NEEDS FIRMWARE API** (disabled at input.c:64)

### Display Ownership (Critical Blocker)
The firmware draws its UI in the main event loop BEFORE calling Frame_Handler. To render game graphics:

1. Set bit 3 of SFR address `0x0D53` to disable firmware drawing
2. Load custom VGA palette (4-bit RAMDAC, shift 6-bit values >> 2)
3. Write directly to VRAM at 0x1A0000 (320x240 linear framebuffer)

The MN89304 VGA controller uses a 4-bit RAMDAC (not standard 6-bit VGA). Key registers:
- DAC write index: 0x1703C8
- DAC data: 0x1703C9 (write R,G,B sequentially)
- CRTC index/data: 0x1703D4/0x1703D5

### DISK MENU Activation (Implemented)
When user selects our DISK MENU entry, firmware posts event `0x01E0009C` via `ApPostEvent`. The dispatch system (FA9660) routes this to our registered handler `0x016A`, which calls `Mines_Handler`. This sets `GAME_ACTIVE=1`, and the next `Frame_Handler` call initializes the C runtime and starts the game. On quit, display ownership is returned to firmware.

### Workspace Pointer System
Boot_Init receives workspace pointer `0x027ED2` in XWA. Key offsets:
- `workspace[+0x0E0A]` -> Handler Table A (registration, callbacks, display)
- `workspace[+0x0E88]` -> Handler Table B (init functions)
- Table A `[+0x02C4]` -> DISK MENU registration function
- Table A `[+0x00E4]` -> Handler registration function
- Table A `[+0x0124]` -> Display callback
- Table A `[+0x0244/0x0248/0x024C]` -> UI callbacks

### LLVM Backend Bugs (Active Workarounds)
- Bug #10: Register x/y swap on inlining — `__attribute__((noinline))` on tile_vram_ptr
- Bug #11: for-loop with uint16_t counter exits after 1 iteration — use do-while + uint32_t

### LLVM Assembler Limitations
Remaining unsupported features (not bugs):
- `ei` / `di` instructions: use raw `.byte` opcodes
- Large displacement loads (>127 bytes): use `add + ld (xreg)` workaround
- `ld a, (addr)` direct byte load: use register-indirect

## Research Documentation
- Extension board interface: `~/devel/kn5000-docs/hdae5000-homebrew.md`
- Display subsystem: `~/devel/kn5000-docs/display-subsystem.md`
- Original HDAE5000 firmware: `~/devel/kn5000-docs/hdae5000.md`
- ROM disassembly: `~/devel/kn5000-roms-disasm/`

## Next Steps (in order)
1. Re-enable control panel input (remove early return at input.c:64)
2. Test DISK MENU activation via actual button press (not Lua)
3. Implement firmware-mediated input (workspace UI callbacks)
4. Handle game exit (return display to firmware)
