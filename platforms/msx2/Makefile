export PATH := $(PWD)/../../bin:$(PATH)
Z80_CFLAGS := $(Z80_CFLAGS) -DUNUSED_MACRO -DMINEFIELD_ALLOCATION
ifeq ($(DEBUG),1)
    Z80_CFLAGS := $(Z80_CFLAGS) -DUSE_DEBUG_MODE
else
    Z80_CFLAGS := $(Z80_CFLAGS)
endif

SCREEN_LAYOUT := -DSCREEN_WIDTH=32 -DSCREEN_HEIGHT=26 -DMINEFIELD_X_OFFSET=1 -DMINEFIELD_Y_OFFSET=2 \
	-DHOURGLASS_X_POS=24 -DHOURGLASS_Y_POS=14 -DBOMB_ICON_X_POS=24 -DBOMB_ICON_Y_POS=11 -DENABLE_TIMER \
	-DENABLE_COUNTER
# 16x16 tiles by default
ifndef _8X8_TILES
    SCREEN_LAYOUT := $(SCREEN_LAYOUT) -D_16X16_TILES
    TILE_FILE := classic.xpm
else
    SCREEN_LAYOUT := $(SCREEN_LAYOUT) -D_8X8_TILES
    TILE_FILE := mines8x8.xpm
endif

MAKE := make
Z80_CC := sdcc
Z80_AS := sdasz80
HEXTOOL := hex2bin
#HEXTOOL := ./tools/MSXhex
SC5TOOL := ./tools/SCREEN5
BINTOOL := ./tools/MSXbin

BUILDDIR := build
TARGET := $(BUILDDIR)/mines
CODE := 0x4000
DATA := 0xc000
ROM_MAX := 0x8000
Z80_CFLAGS := $(Z80_CFLAGS) -mz80 --Werror --fsigned-char --std-sdcc99 --opt-code-size  --fomit-frame-pointer $(EXTRA_Z80_CFLAGS) -I. -I../../common
Z80_LDFLAGS := --no-std-crt0

C_SOURCES := $(wildcard *.c)
ASM_SOURCES := $(wildcard *.asm)
HEADERS := $(wildcard *.h)
OBJECTS := $(patsubst %.c,$(BUILDDIR)/%.rel,$(C_SOURCES)) $(patsubst %.asm,$(BUILDDIR)/%.rel,$(ASM_SOURCES))
COMMON := $(patsubst ../../common/%.c,$(BUILDDIR)/%.rel,$(wildcard ../../common/*.c)) \
          $(patsubst ../../common/16x16/%.c,$(BUILDDIR)/16x16/%.rel,$(wildcard ../../common/16x16/*.c))
Z80_LIBS :=

.PHONY: all clean tools openmsx run
all: $(TARGET).rom
$(TARGET).rom: $(OBJECTS) $(COMMON) $(BUILDDIR)/crt0.rel
	$(Z80_CC) $(Z80_CFLAGS) $(Z80_LDFLAGS) $(Z80_LIBS) --code-loc $(CODE) --data-loc $(DATA) $(BUILDDIR)/crt0.rel $(OBJECTS) $(COMMON) -o $(TARGET).ihx
	#$(HEXTOOL) $(TARGET).ihx -o $(TARGET).rom -e ROM -p 0xFF -s 0x4000 -l $(ROM_MAX)
	$(HEXTOOL) -e bin -p FF -l $(ROM_MAX) $(TARGET).ihx
	@cp $(TARGET).bin $(TARGET).rom

openmsx: run

run: $(TARGET).rom
	openmsx -carta $(TARGET).rom -machine C-BIOS_MSX2 -script debugdevice.tcl

$(BUILDDIR)/16x16/%.rel: ../../common/16x16/%.c $(BUILDDIR) ../../common/common.h
	$(Z80_CC) $(Z80_CFLAGS) $(SCREEN_LAYOUT) -c $< -o $@

$(BUILDDIR)/%.rel: ../../common/%.c $(BUILDDIR) ../../common/common.h
	$(Z80_CC) $(Z80_CFLAGS) $(SCREEN_LAYOUT) -c $< -o $@

$(BUILDDIR)/%.rel: %.c $(HEADERS) $(BUILDDIR)
	$(Z80_CC) $(Z80_CFLAGS) $(SCREEN_LAYOUT) -c $< -o $@

$(BUILDDIR)/%.rel: %.asm $(HEADERS) $(BUILDDIR)
	$(Z80_AS) -g -o $@ $<

$(BUILDDIR)/crt0.rel: crt0.z80
	$(Z80_AS) -g -o $@ $<

$(BUILDDIR)/video.rel: video.c $(BUILDDIR)/mines.h
	$(Z80_CC) $(Z80_CFLAGS) $(SCREEN_LAYOUT) -c $< -o $@

.DELETE_ON_ERROR:
$(BUILDDIR)/mines.h: $(TILE_FILE) $(SC5TOOL)
	$(SC5TOOL) mines > $@

$(BUILDDIR):
	mkdir -p $(BUILDDIR)/16x16

$(SC5TOOL): tools/SCREEN5.c
	XPM=$(TILE_FILE) make -C tools

clean:
	rm -rf $(OBJECTS) $(COMMON) build
	make -C tools clean
