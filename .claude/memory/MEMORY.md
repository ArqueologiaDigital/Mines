# Mines KN5000 Project Memory

## Project Structure
- Platform code: `platforms/kn5000/` (startup.s, kn5000.ld, Makefile, C sources)
- Common code: `common/` (main.c, minefield.c, tiles.c)
- LLVM toolchain: `../llvm-project/build/bin/`
- Build: pure LLVM pipeline (clang, llvm-link, llc, ld.lld, llvm-objcopy)
- Target: TMP94C241F (TLCS-900/H2) @ 16 MHz

## LLVM TLCS-900 Encoding Bugs — 8 Fixed, 2 Active, 1 HW Characteristic
See [llvm-encoding-bugs.md](llvm-encoding-bugs.md) for full bug documentation.
See [workarounds.md](workarounds.md) for C-level workarounds + clean code to restore.

**Recently fixed (rebuild LLVM to pick up):**
- Bug #8: 8-bit register encoding FIXED (commit 4903b603025d)
- Bug #9: F3/F2 prefix NOT A BUG (valid TLCS-900/H2 complex addressing prefix)

**Active bugs requiring C workarounds:**
- Bug #10: Register x/y swap on inlining — use __attribute__((noinline))
- Bug #11: for-loop with uint16_t counter exits after 1 iteration — use do-while + uint32_t

**Remaining assembler limitations** (not bugs, just unsupported):
- Large displacement loads (>127 bytes): use `add + ld (xreg)` workaround
- `ld a, (addr)` byte direct memory load: use register-indirect
- `ei` / `di` mnemonics not supported: use raw `.byte` opcodes

## TLCS-900 Opcode Quick Reference
- 0x1B = JP I24 (24-bit absolute jump)
- 0x38-0x3E = PUSH long (32-bit), 0x58-0x5E = POP long (32-bit)
- 0x48-0x4E = POP word (16-bit) — NOT push!
- A0-A7 = register-indirect prefix (load group)
- B0-B7 = register-indirect prefix (store + CALL/JP group)
- CALL T (unconditional) = sub-opcode 0xE8 in b0 table
- MAME uses 900htbl.hxx tables (TLCS-900/H variant), NOT 900tbl.hxx

## KN5000 / HDAE5000 Architecture
- ROM: 0x280000-0x2FFFFF (512KB extension ROM)
- RAM: 0x200000-0x27FFFF (256KB extension DRAM in MAME)
- VRAM: 0x1A0000-0x1DFFFF (256KB, 320x240 LCD, 8bpp)
- AudioMix debug: addr port 0x150000, data port 0x150002
- XAPR header: offset 0x00; Boot_Init: offset 0x08; Frame_Handler: offset 0x10
- Frame_Handler called by firmware ~14s after Boot_Init completes
- Workspace pointer 0x027ED2 passed in XWA at Boot_Init, stored to 0x200008

## Display System (Working!)
- VRAM writes from extension code work directly
- MN89304 VGA controller: 4-bit RAMDAC, 320x240 8bpp linear framebuffer
- Palette load: DAC auto-increment, set index 0 at 0x1703C8, write RGB to 0x1703C9
- VRAM clear: assembly clear_vram uses SUB for counter (DEC doesn't set flags)
- VRAM @ 0x1A0000 mapped via VGA mem_linear_r/w (direct byte access)
- **Display ownership**: Firmware renders BEFORE Frame_Handler in main loop.
  Game counters by reloading palette + clearing VRAM + full minefield redraw
  every frame in idle_update(), right before yield_to_firmware(). ~22 FPS.
- Firmware display gate at 0xD53 bit 3 was always 0 during testing (not useful).
  Firmware VRAM writes come from an unidentified rendering path.

## MAME Testing (Key Findings)
- MUST use `-extension hdae5000` to load extension board (not default!)
- MUST use `-skip_gameinfo` to bypass splash screens for automated testing
- Headless: `SDL_VIDEODRIVER=offscreen SDL_AUDIODRIVER=dummy -video soft -nowindow -sound none`
- Auto-exit: `-seconds_to_run N`
- Screenshot: `manager.machine.video:snapshot()` from Lua
- Snapshot dir: `-snapshot_directory /tmp/mame_snaps`
- MAME Lua: use `emu.register_frame_done(function() ... end)` for frame callbacks
  Access memory: `manager.machine.devices[":maincpu"].spaces["program"]:read_u32(addr)`

## DISK MENU Activation Events (Fully Working!)
- Button-press activation event: **0x01C00008** (sent when user selects entry via LCD button)
- Direct event injection: **0x01E0009C** (via ApPostEvent)
- Both intercepted by Mines_Handler in startup.s
- DISK MENU navigation: EXIT→EXIT→MENU:DISK→RIGHT 5 selects "Mines Game"
- Events received by handler on selection: 0x01C00008, 0x01C00039, 0x01C00002, 0x01E00014
- Events on menu open: 0x01C00001, 0x01E00014, 0x01C0000F
- On activation, handler skips default delegate (prevents "FD SAVE/LOAD TEST" screen)

## Current Implementation Status
- DISK MENU entry with mine icon + "Mines Game" text: **WORKING**
- Game activation from DISK MENU button press: **WORKING** (event 0x01C00008)
- Display/rendering to LCD: **WORKING** (full minefield visible on black bg)
- Cooperative multitasking: **WORKING** (yield_to_firmware/resume cycle)
- Game loop: **RUNNING** (gameplay_update state, PROGRESS=0xD7, ~22 FPS)
- Palette loading: **WORKING** (game colors visible, reloaded per frame)
- VRAM ownership: **WORKING** (per-frame clear + redraw beats firmware)
- Control panel input: disabled (return MINE_INPUT_IGNORED at input.c:64)
- Auto-activate in Boot_Init for headless testing (bypasses DISK MENU)
- NEEDS: input re-enable, game exit handling

## Build & Test
- `make` in platforms/kn5000/ builds ROM + installs to MAME ROM set
- `make test` runs in MAME (needs Qt5 display)
- Headless test command:
  ```
  cd /mnt/shared/mame && QT_QPA_PLATFORM=offscreen \
  LD_LIBRARY_PATH=/tmp/qt5libs/usr/lib/x86_64-linux-gnu:$LD_LIBRARY_PATH \
  ./mame kn5000 -rompath "roms;../custom_kn5000_roms/mines" \
  -extension hdae5000 -skip_gameinfo -video none -sound none \
  -autoboot_script /tmp/test.lua -seconds_to_run 25
  ```
- Debug markers: B1 = Boot_Init, F1 = Frame_Handler, A1 = Game Activation
- MAME binary: `/mnt/shared/mame/mame` (symlink to fs_mame)

## Documentation
- kn5000-docs: `/mnt/shared/kn5000-docs/` (Jekyll site)
- Key pages updated: hdae5000-homebrew.md, display-subsystem.md
- Disassembly project: `/mnt/shared/kn5000-roms-disasm/`
