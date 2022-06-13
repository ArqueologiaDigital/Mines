export PATH := $(PWD)/../../bin:$(PATH)

ifeq ($(DEBUG),1)
    Z80_CFLAGS := $(Z80_CFLAGS) -DUSE_DEBUG_MODE
else
    Z80_CFLAGS := $(Z80_CFLAGS)
endif

Z80_CC := sdcc
Z80_AS := sdasz80
HEXTOOL := hex2bin
#HEXTOOL := ./tools/MSXhex
BINTOOL := ./tools/MSXbin

BUILDDIR := build
TARGET := $(BUILDDIR)/minesweeper
CODE := 0x4000
DATA := 0xc000
ROM_MAX := 0x8000
Z80_CFLAGS := $(Z80_CFLAGS) -mz80 --Werror --fsigned-char --std-sdcc99 --opt-code-size  --fomit-frame-pointer $(EXTRA_Z80_CFLAGS) -I. -I../../common
Z80_LDFLAGS := --no-std-crt0

C_SOURCES := $(wildcard *.c)
ASM_SOURCES := $(wildcard *.asm)
HEADERS := $(wildcard *.h)
OBJECTS := $(patsubst %.c,$(BUILDDIR)/%.rel,$(C_SOURCES)) $(patsubst %.asm,$(BUILDDIR)/%.rel,$(ASM_SOURCES))
COMMON := $(patsubst ../../common/%.c,$(BUILDDIR)/%.rel,$(wildcard ../../common/*.c))
Z80_LIBS :=

.PHONY: all clean tools openmsx run
all: tools $(TARGET).rom

tools: $(BINTOOL)
	LDFLAGS= CFLAGS= make -C tools

SCREEN_RESOLUTION := -DSCREEN_WIDTH=32 -DSCREEN_HEIGHT=26 -DMINEFIELD_X_OFFSET=6 -DMINEFIELD_Y_OFFSET=3
$(TARGET).rom: $(OBJECTS) $(COMMON) $(BUILDDIR)/crt0.rel
	$(Z80_CC) $(Z80_CFLAGS) $(Z80_LDFLAGS) $(Z80_LIBS) --code-loc $(CODE) --data-loc $(DATA) $(BUILDDIR)/crt0.rel $(OBJECTS) $(COMMON) -o $(TARGET).ihx
	#$(HEXTOOL) $(TARGET).ihx -o $(TARGET).rom -e ROM -p 0xFF -s 0x4000 -l $(ROM_MAX)
	$(HEXTOOL) -e bin -p FF -l $(ROM_MAX) $(TARGET).ihx
	@cp $(TARGET).bin $(TARGET).rom

openmsx: run

run: $(TARGET).rom
	openmsx -carta $(TARGET).rom -machine C-BIOS_MSX2 -script debugdevice.tcl

$(BUILDDIR)/%.rel: ../../common/%.c $(BUILDDIR) ../../common/common.h
	$(Z80_CC) $(Z80_CFLAGS) $(SCREEN_RESOLUTION) -c $< -o $@

$(BUILDDIR)/%.rel: %.c $(HEADERS) $(BUILDDIR)
	$(Z80_CC) $(Z80_CFLAGS) -c $< -o $@

$(BUILDDIR)/%.rel: %.asm $(HEADERS) $(BUILDDIR)
	$(Z80_AS) -g -o $@ $<

$(BUILDDIR)/crt0.rel: crt0.z80
	$(Z80_AS) -g -o $@ $<

$(BUILDDIR)/video.rel: video.c mines.h
	$(Z80_CC) $(Z80_CFLAGS) -c $< -o $@

mines.h: MINES.SC5 $(BINTOOL)
	$(BINTOOL) $< -skip 0 7 -o $@ -t mines_data -n 12

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

clean:
	rm -rf $(OBJECTS) $(COMMON) build $(BINTOOL) mines.h
