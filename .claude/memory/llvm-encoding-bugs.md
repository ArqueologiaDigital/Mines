# LLVM TLCS-900 Backend Encoding Bugs

## Status: 9 Fixed + 1 Confirmed Not-a-Bug + 1 Not Reproducible + 1 HW Characteristic (fixed in LLVM)

---

## FIXED BUGS (1-8)

### Bug 1: Direct memory load prefix (F2 vs E2) — FIXED
- Was: `ld reg, (addr)` used F2 prefix instead of E2
- Now generates correct E2 prefix

### Bug 2: Immediate-to-memory store sub-opcode — FIXED
- Was: `ld (addr), imm32` emitted sub-opcode 0x08
- Now generates correct encoding

### Bug 3: Indirect CALL sub-opcode — FIXED
- Was: `call (xreg)` emitted sub-opcode 0x1F (undefined)
- Now generates 0xE8 (CALL T = unconditional)

### Bug 4: JP opcode — FIXED
- Was: JP encoded as 0x1C (CALL I16)
- Now generates 0x1B (JP I24)

### Bug 5: PUSH encoding — FIXED
- Was: `push xwa` generated 0x48 (POP word 16-bit!) instead of 0x38 (PUSH long 32-bit)
- Now generates correct 0x38-0x3E for 32-bit push

### Bug 6: Source memory prefix always used 32-bit (0xA0) — FIXED
- Was: 8/16-bit register-indirect loads used 0xA0-0xA7 (32-bit prefix)
- Now uses 0x80-0x87 for byte, 0x90-0x97 for word, 0xA0-0xA7 for long
- ISel auto-materializes address for byte/word global loads (LD32ri + reg-indirect)
- Displacement range restricted from ±32767 to ±127 (d8) in ISel

### Bug 7: INC/DEC immediate field off-by-one — FIXED
- Was: `inc 1, xreg` encoded I3=0 (which means increment by 8, not 1!)
- TLCS-900 I3 convention: 000=8, 001=1, 010=2, ..., 111=7
- LLVM used `(Count - 1) & 7` → should be `Count & 7`
- Fixed in both encoder and disassembler

### Bug 8: 8-bit Register Encoding Returns Class Index Instead of HWEncoding — FIXED
- Was: `getRegEncoding()` returned GPR_lo8 class index (A=0, C=1, E=2, L=3)
  instead of sub-register HWEncoding (A=1, C=3, E=5, L=7)
- Root cause: GPR_lo8 operands contain 32-bit parent registers (XWA/XBC/XDE/XHL),
  and the encoder used the parent's HWEncoding instead of the 8-bit sub-register's
- Fix: `getRegEncoding()` now extracts the sub_8bit sub-register's HWEncoding
- Commit: `4903b603025d` "Fix 8-bit register encoding: use sub-register HWEncoding"
- C workarounds can be reverted once LLVM is rebuilt (see workarounds.md)

---

## CONFIRMED NOT A BUG

### Former Bug 9: F3/F2 Memory Prefix — NOT A BUG
- Was believed: F3 prefix conflicts with SWI 3 (Software Interrupt)
- Actually: On the TLCS-900/H2 variant, opcodes 0xF0-0xF5 dispatch to the
  s_mnemonic_f0 sub-table for complex memory addressing modes:
  - 8-bit address, 16-bit address, 24-bit address
  - reg+d16, reg+reg addressing
  - Pre-decrement, post-increment
- SWI instructions are at 0xF8-0xFF on the TLCS-900/H2 (NOT 0xF0-0xF7)
- MAME's decode tables confirm this (TLCS-900/H2 uses 900htbl.hxx)
- **No workaround needed**: direct memory stores with F2/F3 prefix are valid

---

## RESOLVED BUG 10: Register Allocation X/Y Swap — NOT REPRODUCIBLE (Feb 28)

**Status:** NOT REPRODUCIBLE — original analysis was based on incorrect register-parameter mapping.

### Original Report

Tiles were reported as drawn at transposed positions when `tile_vram_ptr` was
inlined into `set_tile`. The analysis assumed XBC held the first parameter (dst_x)
and XDE held the second (dst_y).

### Resolution

The calling convention has always been XDE-first, XBC-second:
`CCAssignToReg<[XDE, XBC, XIX, XIY]>` in TLCS900CallingConv.td.

With the correct mapping (XDE=dst_x, XBC=dst_y):
- `mul xbc, 2560` correctly computes dst_y * 2560 (row offset)
- `sla xde, 3` correctly computes dst_x * 8 (column offset)

Verified with constant-folded `set_tile(5, 10, 42)` → address 0x1A6428
= VRAM_BASE + 10*2560 + 5*8 (correct).

### Workaround (can be removed)

The `__attribute__((noinline))` on `tile_vram_ptr` in Mines video.c is
no longer needed. Remove it and verify on MAME

---

## FIXED BUG 11: For-Loop with uint16_t Counter Exits After 1 Iteration — FIXED (Feb 28)

**FIXED:** Commit `eba2fe6622ee` "Fix Bug #11: EXTS/EXTZ don't set flags on TLCS-900/H"

### Root Cause

EXTS32 and EXTZ32 were declared with `Defs=[SR]` in TLCS900InstrInfo.td, telling the
compiler they set flags. On TLCS-900/H hardware, EXTS/EXTZ do NOT set any flags
(confirmed via MAME's op_EXTZLR and op_EXTSLR implementations).

For uint16_t loops, clang generates a count-down pattern: DEC + EXTZ + CP 0 + JR NZ.
The RedundantCmpElim pass saw EXTZ (which it thought set Z) followed by CP 0 + JPcc NZ,
and removed the CP as "redundant." This left the JR NZ reading stale flags, causing
immediate loop exit.

### Fix

1. Moved EXTS32/EXTZ32 out of the `Defs=[SR]` block in TLCS900InstrInfo.td
2. Removed EXTS32/EXTZ32 from `isFlagSettingDef()` in RedundantCmpElim
3. Added regression test: `llvm/test/CodeGen/TLCS900/bug11-i16-loop.ll`

### C-level Workaround (can be reverted)

The workaround of using `do-while` with `uint32_t` counter is no longer needed.
Standard `for` loops with `uint16_t` counters now work correctly

---

## HARDWARE CHARACTERISTIC: 16/32-bit INC/DEC Don't Set Flags

**Severity:** CRITICAL for assembly programmers; C compiler currently safe
**This is NOT an LLVM bug — it is a TLCS-900/H hardware characteristic.**

### Problem

On TLCS-900/H, the `INC #, R` and `DEC #, R` instructions **only set flags (S, Z, H, V)
for 8-bit register operands**. When used with 16-bit (BC, DE, HL, etc.) or 32-bit
(XBC, XDE, XHL, etc.) registers, **no flags are modified at all**.

This means:
```asm
; WRONG — infinite loop! Z flag is never set by DEC on 32-bit register
ld    xbc, 100
.loop:
dec   1, xbc
jrl   nz, .loop     ; Z flag is stale — loop never terminates

; CORRECT — use SUB which always sets flags
ld    xbc, 100
.loop:
sub   xbc, 1
jrl   nz, .loop     ; Z flag properly set by SUB

; ALSO CORRECT — explicit zero test after DEC (firmware pattern)
ld    xbc, 100
.loop:
dec   1, xbc
or    xbc, xbc       ; sets Z flag based on register value
jrl   nz, .loop
```

### Evidence

- Firmware disassembly: all `dec 1, xbc` instances are followed by `or xbc, xbc`
  (explicit zero test), confirming the firmware author knew about this behavior.
- MAME's TLCS-900 emulation correctly implements this — `op_DECWIR()` and
  `op_DECLIR()` do raw subtraction without flag updates, matching hardware spec.
- Verified empirically: `dec 1, xbc` + `jrl nz` creates infinite loop in MAME,
  while `sub xbc, 1` + `jrl nz` terminates correctly.

### LLVM Backend Status — FIXED (Feb 24)

**FIXED:** Moved INC16/DEC16/INC32/DEC32 outside their enclosing `let Defs = [SR]`
blocks in TLCS900InstrInfo.td (commit ef7fa649c5d8). The compiler now correctly knows
these instructions don't produce flag outputs and won't generate DEC+branch patterns.
INC8/DEC8 remain inside `Defs = [SR]` since 8-bit INC/DEC correctly set flags.

### Affected Code

- `clear_vram` in startup.s: uses `sub xbc, 1` (correct) instead of `dec 1, xbc`
- Copy_C_Data and Clear_C_BSS loops in startup.s: use `sub xbc, 4` (correct)
- All C-compiled loops: currently safe (compiler uses separate CP instructions)

---

## Remaining Assembler Limitations (not bugs)

- Large displacement loads (>127 bytes): assembler rejects with "displacement too large".
  Use `add xreg, disp; ld xreg, (xreg)` workaround.
- Byte-size direct memory loads (`ld a, (addr)`) in assembly: not supported.
  Use register-indirect: `ld xhl, addr; ld xwa, (xhl)`.
  (C compiler handles this automatically via ISel patterns.)
