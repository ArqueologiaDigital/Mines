export PATH := $(PWD)/../../bin:$(PATH)
CC := sdcc
AS := sdasz80
HEX2BIN := hex2bin

TMPDIR := build
TARGET := minesweeper.rom
CODE := 0x4000
DATA := 0xc000
ROM_MAX := 0x8000
CFLAGS := -mz80 --Werror --fsigned-char --std-sdcc99 --opt-code-speed --fomit-frame-pointer $(EXTRA_CFLAGS)
LDFLAGS := --no-std-crt0

SOURCES := $(wildcard *.c)
OBJECTS := $(patsubst %.c,$(TMPDIR)/%.rel,$(SOURCES))
OBJECTS := $(filter-out $(TMPDIR)/crt0.rel,$(OBJECTS))

COMMON  := $(wildcard ../../common/*.c)
C_OBJECTS := $(patsubst ../../common/%.c,$(TMPDIR)/%.rel,$(COMMON))

LIBS :=

$(TARGET): $(OBJECTS) $(C_OBJECTS) $(TMPDIR)/crt0.rel
	$(CC) $(CFLAGS) $(LDFLAGS) $(LIBS) --code-loc $(CODE) --data-loc $(DATA) $(TMPDIR)/crt0.rel $(OBJECTS) $(C_OBJECTS) -o $(TARGET).ihx
	$(HEX2BIN) -e bin -p 00 -l $(ROM_MAX) $(TARGET).ihx
	@cp $(TARGET).bin $(TARGET).rom

openmsx: $(TARGET)
	openmsx -carta $(TARGET) -machine msx1

$(TMPDIR)/%.rel: ../../common/%.c
	mkdir -p build
	$(CC) $(CFLAGS) $(LDFLAGS) -c $< -o $@

$(TMPDIR)/%.rel: %.c
	mkdir -p build
	$(CC) $(CFLAGS) $(LDFLAGS) -c $< -o $@

$(TMPDIR)/%.rel: %.z80
	mkdir -p build
	$(AS) -g -o $@ $<

.PHONY: clean
clean:
	rm -rf $(OBJECTS) $(C_OBJECTS) $(TARGET) build $(TARGET).*
