# C-Level Workarounds for LLVM TLCS-900 Bugs

## Status: ALL WORKAROUNDS REMOVED (Feb 28, 2026)

All LLVM TLCS-900 bugs (#8, #9, #10, #11) are fixed or resolved. All C-level workarounds
have been removed from the Mines codebase. The code now uses clean, natural C patterns.

### What was removed (commit 8b1d85e):

1. **32-bit tile copy helpers** (Bug #8) → replaced with simple byte-level `VRAM_BASE[offset + x] = pixel`
2. **`__attribute__((noinline))` on tile_vram_ptr** (Bug #10) → replaced with inline `tile_vram_offset()` using multiply
3. **`__attribute__((noinline))` on copy_tile_row/copy_tile_row_masked** (Bug #8) → functions removed entirely
4. **Palette auto-increment workaround** (Bug #8/#9) → simplified to use VGA register addresses directly
5. **Workaround comments** → all cleaned up

### Verification

Game renders correctly on MAME after workaround removal (verified with automated Lua screenshot test).
