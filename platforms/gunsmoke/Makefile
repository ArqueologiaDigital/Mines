export PATH := ~/mame:$(PATH)

MAME := mame
ROMPATH := ~/nonfree/mame
Z80ASSEMBLER = sdasz80
Z80COMPILER = sdcc -mz80
OBJCOPY = objcopy
CC = gcc -std=gnu99
PCB_VARIANT = gunsmoke
ROMSET = $(BUILDDIR)/3.ic85 $(BUILDDIR)/gs03.09n $(BUILDDIR)/gs01.11f #gs14.11c g-01.03b g-02.04b g-03.05b gs13.06c gs12.05c gs11.04c gs10.02c gs09.06a gs08.05a gs07.04a gs06.02a
VIDEO_RESOLUTION := -DSCREEN_WIDTH=28 -DSCREEN_HEIGHT=32 -DMINEFIELD_X_OFFSET=3 -DMINEFIELD_Y_OFFSET=5

BUILDDIR := build
SOURCES := $(wildcard *.c)
OBJECTS := $(patsubst %.c,$(BUILDDIR)/%.rel,$(SOURCES))
OBJECTS := $(filter-out $(BUILDDIR)/crt0.rel,$(OBJECTS))

COMMON  := $(wildcard ../../common/*.c)
C_OBJECTS := $(patsubst ../../common/%.c,$(BUILDDIR)/%.rel,$(COMMON))

Z80_EXTRA_CFLAGS := -DUNUSED_MACRO
Z80_CFLAGS := -mz80 --Werror --fsigned-char --std-sdcc99 --opt-code-speed --fomit-frame-pointer $(Z80_EXTRA_CFLAGS) -I../../common
Z80_LDFLAGS := --no-std-crt0
CODE := 0x0080
DATA := 0xE000

.PHONY: all run debug install clean
all: $(ROMSET)

run: $(ROMSET) install
	$(MAME) -rp $(ROMPATH) $(PCB_VARIANT) -window

debug: $(ROMSET) install
	$(MAME) -rp $(ROMPATH) $(PCB_VARIANT) -window -debug

install: $(ROMSET)
	cp $(BUILDDIR)/gs01.11f $(ROMPATH)/gunsmoke/
	cp $(BUILDDIR)/3.ic85 $(ROMPATH)/gunsmoke/
	cp $(BUILDDIR)/gs03.09n $(ROMPATH)/gunsmoke/

$(BUILDDIR)/gs01.11f: $(BUILDDIR)/gs01.11f.orig tools/patch_CGROM
	cp $< $@
	./tools/patch_CGROM

$(BUILDDIR)/gs01.11f.orig: $(ROMPATH)/gunsmoke/gs01.11f tools/patch_CGROM
	cp $< $@

# This is broken, so for now we need to export png manually whenever we change artwork:
#mines.png: mines.xcf
#	convert mines.xcf mines.png

mines.xpm: mines.png
	convert mines.png mines.xpm

#mines_background.xpm: mines_background.xcf
#	convert mines_background.xcf mines_background.xpm

tools/patch_CGROM: mines.xpm tools/patch_CGROM.c char_codes.h
	$(CC) -I../../common -I. tools/patch_CGROM.c -o tools/patch_CGROM

#tools/patch_BGROM: mines_background.xpm tools/patch_BGROM.c
#	$(CC) tools/patch_BGROM.c -o tools/patch_BGROM

#gs14.11c: bg_roms

#g-01.03b: bg_roms
#g-02.04b: bg_roms
#g-03.05b: bg_roms

#gs13.06c: bg_roms
#gs12.05c: bg_roms
#gs11.04c: bg_roms
#gs10.02c: bg_roms
#gs09.06a: bg_roms
#gs08.05a: bg_roms
#gs07.04a: bg_roms
#gs06.02a: bg_roms

#bg_roms: tools/patch_BGROM
#	cp $(ROMPATH)/gunsmoke/*.bin $(BUILDDIR)
#	./tools/patch_BGROM
#	cp $(BUILDDIR)/*.bin $(ROMPATH)/gunsmoke/

$(BUILDDIR)/maincpu.ihx: $(OBJECTS) $(C_OBJECTS) $(BUILDDIR)/crt0.rel
	$(Z80COMPILER) $(Z80_CFLAGS) $(Z80_LDFLAGS) $(LIBS) --code-loc $(CODE) --data-loc $(DATA) $(BUILDDIR)/crt0.rel $(OBJECTS) $(C_OBJECTS) -o $(BUILDDIR)/maincpu.ihx

$(BUILDDIR)/%.rel: ../../common/%.c $(BUILDDIR)
	$(Z80COMPILER) $(Z80_CFLAGS) $(Z80_LDFLAGS) $(VIDEO_RESOLUTION) -c $< -o $@

$(BUILDDIR)/%.rel: ./%.c $(BUILDDIR)
	$(Z80COMPILER) $(Z80_CFLAGS) $(Z80_LDFLAGS) $(VIDEO_RESOLUTION) -c $< -o $@

$(BUILDDIR)/crt0.rel: crt0.z80
	$(Z80ASSEMBLER) -g -o $@ $<

$(BUILDDIR)/3.ic85: $(BUILDDIR)/maincpu.ihx
	$(OBJCOPY) $(BUILDDIR)/maincpu.ihx -I ihex -O binary $(BUILDDIR)/3.ic85

$(BUILDDIR)/gs03.09n: $(BUILDDIR)/maincpu.ihx
	$(OBJCOPY) $(BUILDDIR)/maincpu.ihx -I ihex -O binary $(BUILDDIR)/gs03.09n

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

clean:
	rm -rf $(BUILDDIR)
	rm -f tools/patch_CGROM
#	rm -f tools/patch_BGROM
	rm -f mines*.xpm
