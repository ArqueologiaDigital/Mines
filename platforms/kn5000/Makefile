# =============================================================================
# Makefile - Mines for Technics KN5000 (HDAE5000 Extension ROM)
# =============================================================================
# Build pipeline (pure LLVM toolchain):
#   C sources -> clang (-emit-llvm) -> llvm-link (merge .ll) -> llc (-filetype=obj) -> all_c.o
#   startup.s -> clang -c -> startup.o
#   lld (startup.o + all_c.o + linker script) -> ELF
#   llvm-objcopy -O binary -> raw ROM
#   dd pad -> 512KB ROM
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
# LLVM TLCS900 toolchain (built from ../llvm-project/)
LLVM_BIN := ../../../llvm-project/build/bin
CLANG := $(LLVM_BIN)/clang
LLC := $(LLVM_BIN)/llc
OBJCOPY := $(LLVM_BIN)/llvm-objcopy
LLVM_NM := $(LLVM_BIN)/llvm-nm
LLVM_LINK := $(LLVM_BIN)/llvm-link
LLD := $(LLVM_BIN)/ld.lld
LLD_ENV := LD_LIBRARY_PATH=$(LLVM_BIN)/../lib

# Python for tile generation
PYTHON := python3

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

# LLC flags
LLC_FLAGS := -mtriple=tlcs900 -mcpu=tmp94c241 -O2

# Output files
ROM_FILE := $(BUILD_DIR)/mines_kn5000.bin
ELF_FILE := $(BUILD_DIR)/mines_kn5000.elf
DISK_BIN := $(BUILD_DIR)/mines_disk.bin
DISK_ELF := $(BUILD_DIR)/mines_disk.elf
TILES_BIN := $(BUILD_DIR)/tiles.bin
PALETTE_BIN := $(BUILD_DIR)/palette.bin
ICON_BIN := $(BUILD_DIR)/icon.bin

# =============================================================================
# MAME ROM Set Configuration
# =============================================================================
ORIGINAL_ROMS := ../../../kn5000_original_roms/kn5000
ROMSET_BASE := ../../../custom_kn5000_roms/mines
ROMSET_DIR := $(ROMSET_BASE)/kn5000

# Files to copy from original ROM set
ORIGINAL_ROM_FILES := \
	kn5000_v10_program.rom \
	kn5000_subcpu_boot.ic30 \
	kn5000_subprogram_v142_compressed.rom \
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
$(BUILD_DIR)/%.ll: %.c kn5000.h $(COMMON_DIR)/*.h include/*.h | $(BUILD_DIR)
	$(CLANG) $(CFLAGS) -S -emit-llvm -o $@ $<

$(BUILD_DIR)/common_%.ll: $(COMMON_DIR)/%.c $(COMMON_DIR)/*.h include/*.h | $(BUILD_DIR)
	$(CLANG) $(CFLAGS) -S -emit-llvm -o $@ $<

# All C source files compiled to LLVM IR:
COMPILABLE_LL := $(BUILD_DIR)/extras.ll $(BUILD_DIR)/input.ll \
	$(BUILD_DIR)/video.ll $(BUILD_DIR)/common_tiles.ll \
	$(BUILD_DIR)/common_main.ll $(BUILD_DIR)/common_minefield.ll

# =============================================================================
# Step 2: Link all LLVM IR into one module
# =============================================================================
$(BUILD_DIR)/all_c.ll: $(COMPILABLE_LL)
	$(LLVM_LINK) -S -o $@ $^
	@echo "Linked LLVM IR: $$(echo $^ | wc -w) modules -> $@"

# =============================================================================
# Step 3: Compile linked IR to TLCS900 ELF object
# =============================================================================
$(BUILD_DIR)/all_c.o: $(BUILD_DIR)/all_c.ll
	$(LLC) $(LLC_FLAGS) -filetype=obj -o $@ $<
	@echo "Compiled to object: $@"

# =============================================================================
# Step 4: Generate tile and palette data
# =============================================================================
.PHONY: tiles
tiles: $(TILES_BIN) $(PALETTE_BIN) $(ICON_BIN)

$(TILES_BIN) $(PALETTE_BIN) $(ICON_BIN): tools/generate_tiles.py | $(BUILD_DIR)
	$(PYTHON) tools/generate_tiles.py $(BUILD_DIR)

# =============================================================================
# Step 5: Assemble startup code
# =============================================================================
$(BUILD_DIR)/startup.o: startup.s $(TILES_BIN) $(PALETTE_BIN) $(ICON_BIN) | $(BUILD_DIR)
	$(CLANG) -target tlcs900 -mcpu=tmp94c241 -c -o $@ $<

# =============================================================================
# Step 6: Link startup + C code with linker script
# =============================================================================
$(ELF_FILE): $(BUILD_DIR)/startup.o $(BUILD_DIR)/all_c.o kn5000.ld
	$(LLD_ENV) $(LLD) -T kn5000.ld -o $@ $(BUILD_DIR)/startup.o $(BUILD_DIR)/all_c.o
	@echo "Linked ELF: $@"
	@$(LLVM_NM) $@ | grep -E '(main|TILES_DATA|GAME_ACTIVE|__udivsi3)' || true

# =============================================================================
# Step 7: Convert to binary ROM + pad to 512KB
# =============================================================================
$(ROM_FILE): $(ELF_FILE)
	$(OBJCOPY) -O binary $< $@
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
	$(PYTHON) tools/patch_icon.py $(ROMSET_DIR)
	@echo ""
	@echo "ROM set contents:"
	@ls -la $(ROMSET_DIR)/

# =============================================================================
# Test in MAME
# =============================================================================
.PHONY: test
test: romset
	@echo "Running MAME with Mines extension ROM..."
	LD_LIBRARY_PATH=/tmp/qt5libs/usr/lib/x86_64-linux-gnu:$$LD_LIBRARY_PATH /mnt/shared/mame/mame kn5000 -rompath $(ROMSET_BASE) -extension hdae5000 -window -ui_active -oslog 2>&1 | tee ../../../log.txt



# -----------------------------------------------------------------------------
# Note to Claude: Do not edit this target:
fsanches_test: romset
	@echo "Running MAME with Mines extension ROM..."
	mame kn5000 -rompath $(ROMSET_BASE) -extension hdae5000 -window -ui_active -oslog 2>&1 | tee ../../../log.txt
# -----------------------------------------------------------------------------



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
	@echo "--- LLVM TLCS900 Toolchain ---"
	@for tool in clang llc llvm-objcopy llvm-link ld.lld llvm-nm; do \
		if [ -x "$(LLVM_BIN)/$$tool" ]; then \
			echo "  $$tool: $(LLVM_BIN)/$$tool"; \
		else \
			echo "  ERROR: $$tool not found at $(LLVM_BIN)/$$tool"; \
		fi; \
	done
	@echo "--- Python ---"
	@$(PYTHON) --version
	@echo "--- Original ROMs ---"
	@if [ -d "$(ORIGINAL_ROMS)" ]; then \
		echo "  ROM directory: $(ORIGINAL_ROMS)"; \
	else \
		echo "  WARNING: Original ROM directory not found: $(ORIGINAL_ROMS)"; \
	fi

# =============================================================================
# Disk App Build (for App Loader)
# =============================================================================
# Builds a flat binary that the HDAE5000 App Loader can load from hard disk.
# Uses disk_startup.s (no XAPR) and disk.ld (linked for RAM at 0x208000).
#
# The resulting APP.BIN goes into the App Loader's disk image under /APPS/MINES/.
# =============================================================================

.PHONY: disk
disk: $(DISK_BIN)
	@echo ""
	@echo "Disk app built: $(DISK_BIN)"
	@echo "Size: $$(stat -c%s "$(DISK_BIN)" 2>/dev/null || stat -f%z "$(DISK_BIN)" 2>/dev/null) bytes"

# Assemble disk startup (no XAPR, no icon data needed)
$(BUILD_DIR)/disk_startup.o: disk_startup.s $(TILES_BIN) $(PALETTE_BIN) | $(BUILD_DIR)
	$(CLANG) -target tlcs900 -mcpu=tmp94c241 -c -o $@ $<

# Link disk startup + C code with disk linker script
$(DISK_ELF): $(BUILD_DIR)/disk_startup.o $(BUILD_DIR)/all_c.o disk.ld
	$(LLD_ENV) $(LLD) -T disk.ld -o $@ $(BUILD_DIR)/disk_startup.o $(BUILD_DIR)/all_c.o
	@echo "Linked disk ELF: $@"
	@$(LLVM_NM) $@ | grep -E '(main|_start|yield|TILES_DATA|__bss)' || true

# Convert to flat binary (no padding — App Loader reads actual file size)
$(DISK_BIN): $(DISK_ELF)
	$(OBJCOPY) -O binary $< $@
	@echo "Disk app size: $$(stat -c%s "$@" 2>/dev/null || stat -f%z "$@" 2>/dev/null) bytes"

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
	@echo "  C sources -> clang (-emit-llvm) -> llvm-link -> llc (-filetype=obj) -> all_c.o"
	@echo "  startup.s -> clang -c -> startup.o"
	@echo "  lld (startup.o + all_c.o + kn5000.ld) -> ELF -> llvm-objcopy -> 512KB ROM"
