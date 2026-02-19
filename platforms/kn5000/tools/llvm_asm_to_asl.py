#!/usr/bin/env python3
"""Convert LLVM TLCS900 assembly output to ASL-compatible format.

The LLVM backend produces GAS-style assembly that needs adjustments
for Alfred Arnold's ASL Macro Assembler:
  - .Lxxx local labels → .xxx (ASL local label format)
  - GAS directives (.globl, .type, .size, etc.) → removed or converted
  - .data/.text sections → comments
  - .word → DW, .byte → DB
  - .comm → DS (define storage)
  - Shift/rotate operand order: GAS `sla reg, count` → ASL `SLA count, reg`
  - MUL source operand: GAS `mul xrr, xrr2` → ASL `MUL xrr, rr2` (16-bit source)
  - DJNZ with 32-bit register → DEC + JR NZ sequence
  - SET/RES with 32-bit register → use byte sub-register
"""

import re
import sys

# Shift/rotate instructions that need operand order swap
SHIFT_OPS = {'sla', 'srl', 'sra', 'sll', 'rlc', 'rrc', 'rl', 'rr'}

# Map 32-bit register names to their 16-bit sub-registers
XRR_TO_RR = {
    'xwa': 'wa', 'xbc': 'bc', 'xde': 'de', 'xhl': 'hl',
    'xix': 'ix', 'xiy': 'iy', 'xiz': 'iz',
    'XWA': 'WA', 'XBC': 'BC', 'XDE': 'DE', 'XHL': 'HL',
    'XIX': 'IX', 'XIY': 'IY', 'XIZ': 'IZ',
}

# Map 32-bit register names to their low byte
XRR_TO_R = {
    'xwa': 'a', 'xbc': 'c', 'xde': 'e', 'xhl': 'l',
    'XWA': 'A', 'XBC': 'C', 'XDE': 'E', 'XHL': 'L',
}


def is_xrr(reg):
    """Check if register is a 32-bit (XRR) register."""
    return reg.lower() in ('xwa', 'xbc', 'xde', 'xhl', 'xix', 'xiy', 'xiz')


def fix_shift_operands(line):
    """Fix shift/rotate operand order: `sla reg, count` → `SLA count, reg`

    Also splits counts > 16 into multiple instructions since TLCS-900
    only supports shift counts 1-16 in a single instruction.
    Returns a string (single line) or list of strings (multiple lines).
    """
    stripped = line.lstrip()
    indent = line[:len(line) - len(stripped)]

    # Match: <shift_op> <register>, <count>
    # The count can be an immediate or the A register
    m = re.match(r'(\w+)\s+(\w+)\s*,\s*(.+)', stripped)
    if not m:
        return line

    op, arg1, arg2 = m.group(1), m.group(2), m.group(3).strip()

    if op.lower() not in SHIFT_OPS:
        return line

    # arg1 is register, arg2 is count → swap to: op count, register
    # Handle trailing comments
    comment = ''
    if ';' in arg2:
        arg2, comment = arg2.split(';', 1)
        arg2 = arg2.strip()
        comment = '\t; ' + comment.strip()

    # Check if count is numeric and > 16 (needs splitting)
    try:
        count = int(arg2)
        if count > 16:
            # Split into multiple shifts: 16 + remainder
            result = []
            while count > 16:
                result.append(f'{indent}{op}\t16, {arg1}')
                count -= 16
            result.append(f'{indent}{op}\t{count}, {arg1}{comment}')
            return result
    except ValueError:
        pass  # Non-numeric count (e.g., A register), just swap

    return f'{indent}{op}\t{arg2}, {arg1}{comment}'


def fix_mul_operands(line):
    """Fix MUL source operand: `mul xrr1, xrr2` → `MUL xrr1, rr2`

    TLCS-900 MUL is 16×16→32, so source must be 16-bit register.
    The LLVM backend emits 32-bit register names for both operands.
    """
    stripped = line.lstrip()
    indent = line[:len(line) - len(stripped)]

    m = re.match(r'(mul)\s+(\w+)\s*,\s*(\w+)(.*)', stripped, re.IGNORECASE)
    if not m:
        return line

    op, dst, src, rest = m.group(1), m.group(2), m.group(3), m.group(4)

    # If source is a 32-bit register, downsize to 16-bit
    if is_xrr(src) and is_xrr(dst):
        src = XRR_TO_RR.get(src, src)
        return f'{indent}{op}\t{dst}, {src}{rest}'

    return line


def fix_djnz(line):
    """Convert DJNZ with 32-bit register to DEC + JR NZ.

    DJNZ only works with byte/word registers on TLCS-900.
    `djnz xrr, label` → `dec 4, xrr` + `jr nz, label`
    """
    stripped = line.lstrip()
    indent = line[:len(line) - len(stripped)]

    m = re.match(r'djnz\s+(\w+)\s*,\s*(\S+)(.*)', stripped, re.IGNORECASE)
    if not m:
        return None  # Not a djnz instruction

    reg, label, rest = m.group(1), m.group(2), m.group(3)

    if is_xrr(reg):
        # Replace with dec + jr nz
        return [
            f'{indent}dec\t4, {reg}',
            f'{indent}jr\tnz, {label}{rest}'
        ]

    return None  # Not a 32-bit register, keep as-is


def fix_set_res(line):
    """Fix SET/RES with 32-bit register operand.

    SET/RES bit, reg only works on byte/word operands.
    `set N, xrr` → `set N, r` (operate on low byte)
    """
    stripped = line.lstrip()
    indent = line[:len(line) - len(stripped)]

    m = re.match(r'(set|res)\s+(\d+)\s*,\s*(\w+)(.*)', stripped, re.IGNORECASE)
    if not m:
        return line

    op, bit, reg, rest = m.group(1), m.group(2), m.group(3), m.group(4)
    bit_num = int(bit)

    if is_xrr(reg) and bit_num < 8:
        # Use the low byte of the register
        byte_reg = XRR_TO_R.get(reg, reg)
        return f'{indent}{op}\t{bit}, {byte_reg}{rest}'

    return line


def fix_cpl(line):
    """Fix CPL with 32-bit register.

    CPL only works on byte operands. `cpl xrr` → we need to XOR with 0FFFFFFFFh.
    Actually for our use case, cpl xbc where only low byte matters,
    we can use `cpl c` then `extz xbc` but that changes semantics.
    Better: `xor xrr, 0FFFFFFFFh`
    """
    stripped = line.lstrip()
    indent = line[:len(line) - len(stripped)]

    m = re.match(r'cpl\s+(\w+)(.*)', stripped, re.IGNORECASE)
    if not m:
        return line

    reg, rest = m.group(1), m.group(2)

    if is_xrr(reg):
        # Replace with XOR to complement all bits
        return f'{indent}xor\t{reg}, 0FFFFFFFFh{rest}'

    return line


def fix_neg(line):
    """Fix NEG with 32-bit register.

    NEG only works on byte/word. For 32-bit: `xor xrr, 0FFFFFFFFh` + `inc 4, xrr`
    """
    stripped = line.lstrip()
    indent = line[:len(line) - len(stripped)]

    m = re.match(r'neg\s+(\w+)(.*)', stripped, re.IGNORECASE)
    if not m:
        return None

    reg, rest = m.group(1), m.group(2)

    if is_xrr(reg):
        return [
            f'{indent}xor\t{reg}, 0FFFFFFFFh',
            f'{indent}inc\t4, {reg}'
        ]

    return None


def convert(input_path, output_path):
    with open(input_path, 'r') as f:
        lines = f.readlines()

    out = []
    out.append('; Auto-generated from LLVM TLCS900 compiler output')
    out.append('; Source: ' + input_path)
    out.append('')

    for line in lines:
        line = line.rstrip()

        # Skip GAS-only directives
        if any(line.strip().startswith(d) for d in [
            '.file', '.ident', '.section', '.type', '.size',
            '.globl', '.local', '.p2align',
            '.section\t".note'
        ]):
            # Preserve .globl as a comment for reference
            if line.strip().startswith('.globl'):
                sym = line.strip().split()[1] if len(line.strip().split()) > 1 else ''
                out.append('; GLOBAL: ' + sym)
            continue

        # Convert .text section
        if line.strip() == '.text':
            out.append('')
            out.append('; --- Code Section ---')
            continue

        # Convert .data section
        if line.strip() == '.data':
            out.append('')
            out.append('; --- Data Section ---')
            continue

        # Convert .bss section
        if line.strip() == '.bss':
            out.append('')
            out.append('; --- BSS Section ---')
            continue

        # Convert .word to DD (LLVM TLCS900 uses .word for 32-bit values
        # despite the .size directives showing 4 bytes - backend bug)
        m = re.match(r'^(\s*)\.word\s+(.*)', line)
        if m:
            indent, value = m.group(1), m.group(2)
            comment = ''
            if ';' in value:
                value, comment = value.split(';', 1)
                comment = ' ; ' + comment.strip()
            value = value.strip()
            out.append(f'{indent}DD {value}{comment}')
            continue

        # Convert .byte to DB
        m = re.match(r'^(\s*)\.byte\s+(.*)', line)
        if m:
            indent, value = m.group(1), m.group(2)
            out.append(f'{indent}DB {value}')
            continue

        # Convert .comm (uninitialized storage) to DS
        m = re.match(r'^\s*\.comm\s+(\w+)\s*,\s*(\d+)', line)
        if m:
            name, size = m.group(1), m.group(2)
            out.append(f'{name}:\tDS {size}')
            continue

        # Convert .Lxxx local labels to .xxx (ASL format)
        # In labels (at start of line)
        line = re.sub(r'^\.L(\w+):', r'.\1:', line)
        # In references (jump targets, etc.)
        line = re.sub(r'\.L(\w+)', r'.\1', line)

        # Convert empty lines
        if line.strip() == '':
            out.append('')
            continue

        # --- Instruction-level fixes ---

        # Check for DJNZ with 32-bit register (needs multi-line replacement)
        djnz_result = fix_djnz(line)
        if djnz_result is not None:
            if isinstance(djnz_result, list):
                out.extend(djnz_result)
            else:
                out.append(djnz_result)
            continue

        # Check for NEG with 32-bit register
        neg_result = fix_neg(line)
        if neg_result is not None:
            if isinstance(neg_result, list):
                out.extend(neg_result)
            else:
                out.append(neg_result)
            continue

        # Fix shift/rotate operand order (may produce multiple lines)
        shift_result = fix_shift_operands(line)
        if isinstance(shift_result, list):
            # Multi-line shift expansion - apply remaining fixes to each line
            for sline in shift_result:
                sline = fix_mul_operands(sline)
                sline = fix_set_res(sline)
                sline = fix_cpl(sline)
                out.append(sline)
            continue

        line = shift_result

        # Fix MUL source operand size
        line = fix_mul_operands(line)

        # Fix SET/RES with 32-bit register
        line = fix_set_res(line)

        # Fix CPL with 32-bit register
        line = fix_cpl(line)

        out.append(line)

    with open(output_path, 'w') as f:
        f.write('\n'.join(out) + '\n')

    print(f"Converted {len(lines)} lines → {len(out)} lines")


if __name__ == '__main__':
    if len(sys.argv) != 3:
        print(f"Usage: {sys.argv[0]} <input.s> <output.asm>")
        sys.exit(1)
    convert(sys.argv[1], sys.argv[2])
