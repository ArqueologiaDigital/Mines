export PATH := $(PWD)/../../bin:$(PATH)

ifeq ($(DEBUG),1)
    CFLAGS := -DUSE_DEBUG_MODE
else
    CFLAGS :=
endif

CC := sdcc
AS := sdasz80
HEX2BIN := hex2bin

BUILDDIR := build
TARGET := $(BUILDDIR)/minesweeper
CODE := 0x4000
DATA := 0xc000
ROM_MAX := 0x8000
CFLAGS := $(CFLAGS) -mz80 --Werror --fsigned-char --std-sdcc99 --opt-code-speed --fomit-frame-pointer $(EXTRA_CFLAGS) -I../../common
LDFLAGS := --no-std-crt0

C_SOURCES := $(wildcard *.c)
ASM_SOURCES := $(wildcard *.z80)
HEADERS := $(wildcard *.h)
OBJECTS := $(patsubst %.c,$(BUILDDIR)/%.rel,$(C_SOURCES)) $(patsubst %.z80,$(BUILDDIR)/%.rel,$(ASM_SOURCES))
OBJECTS := $(filter-out $(BUILDDIR)/crt0.rel,$(OBJECTS))
COMMON := $(patsubst ../../common/%.c,$(BUILDDIR)/%.rel,$(wildcard ../../common/*.c))
LIBS :=

all: $(TARGET).rom

$(TARGET).rom: $(OBJECTS) $(COMMON) $(BUILDDIR)/crt0.rel
	$(CC) $(CFLAGS) $(LDFLAGS) $(LIBS) --code-loc $(CODE) --data-loc $(DATA) $(BUILDDIR)/crt0.rel $(OBJECTS) $(COMMON) -o $(TARGET).ihx
	$(HEX2BIN) -e bin -p 00 -l $(ROM_MAX) $(TARGET).ihx
	@cp $(TARGET).bin $(TARGET).rom

openmsx: run

debug: $(TARGET).rom
	openmsx -carta $(TARGET).rom -machine C-BIOS_MSX2 -script debugdevice.tcl

run: $(TARGET).rom
	openmsx -carta $(TARGET).rom -machine C-BIOS_MSX2

$(BUILDDIR)/%.rel: ../../common/%.c $(BUILDDIR) ../../common/common.h
	$(CC) $(CFLAGS) $(LDFLAGS) -c $< -o $@

$(BUILDDIR)/%.rel: %.c $(HEADERS) $(BUILDDIR)
	$(CC) $(CFLAGS) $(LDFLAGS) -c $< -o $@

$(BUILDDIR)/%.rel: %.z80 $(HEADERS) $(BUILDDIR)
	$(AS) -g -o $@ $<

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

.PHONY: clean
clean:
	rm -rf $(OBJECTS) $(C_OBJECTS) build
