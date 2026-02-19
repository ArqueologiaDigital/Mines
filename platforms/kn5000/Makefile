# =============================================================================
# Makefile - Mines for Technics KN5000 (HDAE5000 Extension ROM)
# =============================================================================
# Build pipeline:
#   C sources → clang (LLVM IR) → llc (ELF .o) → llvm-objcopy (raw .bin)
#   Python → tile/palette binary data
#   ASL → main.asm (wraps everything into ROM) → p2bin → 512KB ROM
#
# Targets:
#   make          - Build ROM and create MAME ROM set
#   make build    - Build ROM only
#   make romset   - Create MAME ROM set with extension ROM
#   make tiles    - Generate tile and palette data
#   make test     - Run in MAME emulator
#   make clean    - Remove build artifacts
#   make check    - Verify build tools
# =============================================================================

# =============================================================================
# Tool paths
# =============================================================================
# LLVM TLCS900 compiler (built from ../llvm-project/)
LLVM_BIN := ../../../llvm-project/build/bin
CLANG := $(LLVM_BIN)/clang
LLC := $(LLVM_BIN)/llc
OBJCOPY := $(LLVM_BIN)/llvm-objcopy
LLVM_NM := $(LLVM_BIN)/llvm-nm

# ASL Macro Assembler
ASL_PATH := /home/fsanches/devel/tools/asl
ASL := $(ASL_PATH)/asl
P2BIN := $(ASL_PATH)/p2bin

# Python for tile generation
PYTHON := python3

# Shared assembly includes from kn5000-roms-disasm
DISASM_REPO := /home/fsanches/devel/kn5000-roms-disasm
AW_SRC := /home/fsanches/devel/custom-kn5000-roms/anotherworld/src

# =============================================================================
# Build directories and files
# =============================================================================
BUILD_DIR := build
COMMON_DIR := ../../common

# C source files
PLATFORM_C_SRCS := video.c input.c extras.c
COMMON_C_SRCS := $(COMMON_DIR)/main.c $(COMMON_DIR)/minefield.c $(COMMON_DIR)/tiles.c
ALL_C_SRCS := $(PLATFORM_C_SRCS) $(COMMON_C_SRCS)

# C compiler flags
CFLAGS := -target tlcs900 -ffreestanding -nostdlib -O2 \
	-isystem include -I$(COMMON_DIR) -I. \
	-DSCREEN_WIDTH=40 -DSCREEN_HEIGHT=30 \
	-DMINEFIELD_X_OFFSET=10 -DMINEFIELD_Y_OFFSET=4 \
	-DNO_UNUSED_MACRO

# ASL assembler flags
ASL_FLAGS := -w -q -L
ASL_INCLUDES := -i $(DISASM_REPO) -i $(DISASM_REPO)/shared -i $(AW_SRC)/includes -i $(AW_SRC)

# Output files
ROM_FILE := $(BUILD_DIR)/mines_kn5000.bin
C_CODE_ASM := $(BUILD_DIR)/c_code_asl.asm
TILES_BIN := $(BUILD_DIR)/tiles.bin
PALETTE_BIN := $(BUILD_DIR)/palette.bin
ICON_BIN := $(BUILD_DIR)/icon.bin

# =============================================================================
# MAME ROM Set Configuration
# =============================================================================
ORIGINAL_ROMS := /mnt/shared/kn5000_original_roms/kn5000
ROMSET_BASE := /mnt/shared/custom_kn5000_roms/mines
ROMSET_DIR := $(ROMSET_BASE)/kn5000

# Files to copy from original ROM set
ORIGINAL_ROM_FILES := \
	kn5000_v10_program.rom \
	kn5000_subcpu_boot.ic30 \
	kn5000_subprogram_v142.rom \
	kn5000_table_data_rom_even.ic3 \
	kn5000_table_data_rom_odd.ic1 \
	kn5000_rhythm_data_rom.ic14 \
	kn5000_waveform_rom.ic307 \
	kn5000_custom_data_rom.ic19

# The HDAE5000 extension ROM filename for MAME
EXTENSION_ROM_NAME := hd-ae5000_v2_06i.ic4

# =============================================================================
# Default target
# =============================================================================
.PHONY: all
all: romset
	@echo ""
	@echo "Build complete!"
	@echo "ROM: $(ROM_FILE)"
	@echo "ROM set: $(ROMSET_DIR)/"
	@echo "Test with: make test"

# =============================================================================
# Create build directory
# =============================================================================
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# =============================================================================
# Build ROM only
# =============================================================================
.PHONY: build-rom
build-rom: $(ROM_FILE)
	@echo "ROM built: $(ROM_FILE)"
	@echo "Size: $$(stat -c%s "$(ROM_FILE)" 2>/dev/null || stat -f%z "$(ROM_FILE)" 2>/dev/null) bytes"

# =============================================================================
# Step 1: Compile C sources to LLVM IR (individually)
# =============================================================================
# NOTE: The LLVM TLCS900 backend is still in development. Some C files
# compile successfully while others hit codegen bugs:
#   - extras.c:    OK (basic math, memory ops)
#   - input.c:     OK (serial I/O, bit manipulation)
#   - tiles.c:     OK (tile drawing, struct access)
#   - video.c:     FAILS (volatile pointer arithmetic, division)
#   - main.c:      FAILS (switch/jump tables)
#   - minefield.c: FAILS (division/modulo)
#
# Files that compile successfully are assembled together. Functions from
# failing files are provided as hand-written assembly stubs in main.asm.

# Individual LLVM IR generation
$(BUILD_DIR)/%.ll: %.c kn5000.h $(COMMON_DIR)/*.h include/*.h | $(BUILD_DIR)
	$(CLANG) $(CFLAGS) -S -emit-llvm -o $@ $<

$(BUILD_DIR)/common_%.ll: $(COMMON_DIR)/%.c $(COMMON_DIR)/*.h include/*.h | $(BUILD_DIR)
	$(CLANG) $(CFLAGS) -S -emit-llvm -o $@ $<

# Files that the LLVM TLCS900 backend can compile:
COMPILABLE_LL := $(BUILD_DIR)/extras.ll $(BUILD_DIR)/input.ll $(BUILD_DIR)/common_tiles.ll

# =============================================================================
# Step 2: LLVM IR to TLCS900 assembly
# =============================================================================
# NOTE: The LLVM TLCS900 backend can produce assembly output (-filetype=asm)
# but the ELF object emitter (-filetype=obj) has a code emission bug.
# Workaround: produce assembly, then convert to ASL syntax and include
# directly in main.asm via ASL's `include` directive.
$(BUILD_DIR)/%.s: $(BUILD_DIR)/%.ll
	$(LLC) -mtriple=tlcs900 -filetype=asm -O2 -o $@ $<

# Concatenate all compilable assembly files
$(BUILD_DIR)/c_code.s: $(patsubst %.ll,%.s,$(COMPILABLE_LL))
	cat $^ > $@
	@echo "Generated TLCS900 assembly: $$(wc -l < $@) lines from $$(echo $^ | wc -w) files"

# =============================================================================
# Step 3: Convert LLVM assembly to ASL-compatible format
# =============================================================================
# The LLVM assembler output uses GNU as syntax which differs from ASL.
# This script converts it to ASL-compatible TLCS900 assembly.
$(C_CODE_ASM): $(BUILD_DIR)/c_code.s tools/llvm_asm_to_asl.py
	$(PYTHON) tools/llvm_asm_to_asl.py $< $@
	@echo "C code converted to ASL format"

# =============================================================================
# Step 4: Generate tile and palette data
# =============================================================================
.PHONY: tiles
tiles: $(TILES_BIN) $(PALETTE_BIN) $(ICON_BIN)

$(TILES_BIN) $(PALETTE_BIN) $(ICON_BIN): tools/generate_tiles.py | $(BUILD_DIR)
	$(PYTHON) tools/generate_tiles.py $(BUILD_DIR)

# =============================================================================
# Step 5: Assemble ROM wrapper with ASL
# =============================================================================
$(BUILD_DIR)/main.p: main.asm $(C_CODE_ASM) $(TILES_BIN) $(PALETTE_BIN) $(ICON_BIN)
	$(ASL) $(ASL_FLAGS) $(ASL_INCLUDES) -i $(BUILD_DIR) -olist $(BUILD_DIR)/main.listing main.asm -o $@

# =============================================================================
# Step 6: Convert to binary ROM with p2bin
# =============================================================================
$(ROM_FILE): $(BUILD_DIR)/main.p
	$(P2BIN) $< $@ -r 0x280000-0x2FFFFF -l 0xFF
	@SIZE=$$(stat -c%s "$@" 2>/dev/null || stat -f%z "$@" 2>/dev/null); \
	if [ "$$SIZE" -lt 524288 ]; then \
		echo "Padding ROM to 512KB..."; \
		dd if=/dev/zero bs=1 count=$$((524288 - $$SIZE)) 2>/dev/null | tr '\0' '\377' >> $@; \
	fi
	@echo "ROM size: $$(stat -c%s "$@" 2>/dev/null || stat -f%z "$@" 2>/dev/null) bytes"

# =============================================================================
# Create MAME ROM set
# =============================================================================
.PHONY: romset
romset: $(ROM_FILE)
	@echo "Creating MAME ROM set at $(ROMSET_DIR)/"
	mkdir -p $(ROMSET_DIR)
	@for rom in $(ORIGINAL_ROM_FILES); do \
		if [ -f "$(ORIGINAL_ROMS)/$$rom" ]; then \
			cp "$(ORIGINAL_ROMS)/$$rom" "$(ROMSET_DIR)/"; \
		else \
			echo "  WARNING: Missing original ROM: $$rom"; \
		fi; \
	done
	cp $(ROM_FILE) $(ROMSET_DIR)/$(EXTENSION_ROM_NAME)
	@echo "  Installed: $(EXTENSION_ROM_NAME) (Mines game)"
	@echo ""
	@echo "ROM set contents:"
	@ls -la $(ROMSET_DIR)/

# =============================================================================
# Test in MAME
# =============================================================================
.PHONY: test
test: romset
	@echo "Running MAME with Mines extension ROM..."
	mame kn5000 -rompath $(ROMSET_BASE)

# =============================================================================
# Clean
# =============================================================================
.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)
	@echo "Clean complete"

.PHONY: distclean
distclean: clean
	rm -rf $(ROMSET_BASE)
	@echo "ROM set removed"

# =============================================================================
# Verify build tools
# =============================================================================
.PHONY: check
check:
	@echo "Checking build tools..."
	@echo "--- LLVM TLCS900 Compiler ---"
	@if [ -x "$(CLANG)" ]; then \
		echo "  clang: $(CLANG)"; \
	else \
		echo "  ERROR: clang not found at $(CLANG)"; \
	fi
	@if [ -x "$(LLC)" ]; then \
		echo "  llc: $(LLC)"; \
	else \
		echo "  ERROR: llc not found at $(LLC)"; \
	fi
	@if [ -x "$(OBJCOPY)" ]; then \
		echo "  llvm-objcopy: $(OBJCOPY)"; \
	else \
		echo "  ERROR: llvm-objcopy not found at $(OBJCOPY)"; \
	fi
	@echo "--- ASL Assembler ---"
	@if [ -x "$(ASL)" ]; then \
		echo "  asl: $(ASL)"; \
	else \
		echo "  ERROR: asl not found at $(ASL)"; \
	fi
	@if [ -x "$(P2BIN)" ]; then \
		echo "  p2bin: $(P2BIN)"; \
	else \
		echo "  ERROR: p2bin not found at $(P2BIN)"; \
	fi
	@echo "--- Python ---"
	@$(PYTHON) --version
	@echo "--- Original ROMs ---"
	@if [ -d "$(ORIGINAL_ROMS)" ]; then \
		echo "  ROM directory: $(ORIGINAL_ROMS)"; \
	else \
		echo "  WARNING: Original ROM directory not found: $(ORIGINAL_ROMS)"; \
	fi

# =============================================================================
# Help
# =============================================================================
.PHONY: help
help:
	@echo "Mines for KN5000 - Build System"
	@echo ""
	@echo "Build targets:"
	@echo "  make          - Build ROM and create MAME ROM set"
	@echo "  make build    - Build ROM only"
	@echo "  make tiles    - Generate tile/palette data"
	@echo "  make romset   - Create MAME ROM set"
	@echo "  make test     - Run in MAME emulator"
	@echo "  make clean    - Remove build artifacts"
	@echo "  make distclean- Remove everything including ROM set"
	@echo "  make check    - Verify build tools"
	@echo ""
	@echo "Build pipeline:"
	@echo "  C sources → clang → llc → llvm-objcopy → raw binary"
	@echo "  Python → tiles.bin + palette.bin + icon.bin"
	@echo "  ASL → main.asm (wraps everything) → p2bin → 512KB ROM"
