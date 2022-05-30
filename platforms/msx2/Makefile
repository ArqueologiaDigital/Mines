export PATH := $(PWD)/../../bin:$(PATH)
CC := sdcc
AS := sdasz80
HEX2BIN := hex2bin

BUILDDIR := build
TARGET := $(BUILDDIR)/minesweeper
CODE := 0x4000
DATA := 0xc000
ROM_MAX := 0x8000
CFLAGS := -mz80 --Werror --fsigned-char --std-sdcc99 --opt-code-speed --fomit-frame-pointer $(EXTRA_CFLAGS)
LDFLAGS := --no-std-crt0

SOURCES := $(wildcard *.c)
OBJECTS := $(patsubst %.c,$(BUILDDIR)/%.rel,$(SOURCES))
OBJECTS := $(filter-out $(BUILDDIR)/crt0.rel,$(OBJECTS))

COMMON  := $(wildcard ../../common/*.c)
C_OBJECTS := $(patsubst ../../common/%.c,$(BUILDDIR)/%.rel,$(COMMON))

LIBS :=

$(TARGET).rom: $(OBJECTS) $(C_OBJECTS) $(BUILDDIR)/crt0.rel
	$(CC) $(CFLAGS) $(LDFLAGS) $(LIBS) --code-loc $(CODE) --data-loc $(DATA) $(BUILDDIR)/crt0.rel $(OBJECTS) $(C_OBJECTS) -o $(TARGET).ihx
	$(HEX2BIN) -e bin -p 00 -l $(ROM_MAX) $(TARGET).ihx
	@cp $(TARGET).bin $(TARGET).rom

openmsx: $(TARGET).rom
	openmsx -carta $(TARGET).rom -machine C-BIOS_MSX2

$(BUILDDIR)/%.rel: ../../common/%.c $(BUILDDIR)
	$(CC) $(CFLAGS) $(LDFLAGS) -c $< -o $@

$(BUILDDIR)/%.rel: %.c $(BUILDDIR)
	$(CC) $(CFLAGS) $(LDFLAGS) -c $< -o $@

$(BUILDDIR)/%.rel: %.z80 $(BUILDDIR)
	$(AS) -g -o $@ $<

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

.PHONY: clean
clean:
	rm -rf $(OBJECTS) $(C_OBJECTS) build
