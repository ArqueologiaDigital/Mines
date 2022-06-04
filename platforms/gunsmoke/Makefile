MAME = ~/mame/mame
ROMPATH = ~/nonfree/mame
Z80ASSEMBLER = sdasz80
Z80COMPILER = sdcc -mz80
OBJCOPY = objcopy
CC = gcc -std=gnu99
PCB_VARIANT = gunsmoke
ROMSET = 3.ic85 gs03.09n gs01.11f #gs14.11c g-01.03b g-02.04b g-03.05b gs13.06c gs12.05c gs11.04c gs10.02c gs09.06a gs08.05a gs07.04a gs06.02a

BUILDDIR := build
SOURCES := $(wildcard *.c)
OBJECTS := $(patsubst %.c,$(BUILDDIR)/%.rel,$(SOURCES))
OBJECTS := $(filter-out $(BUILDDIR)/boot.rel,$(OBJECTS))

COMMON  := $(wildcard ../../common/*.c)
C_OBJECTS := $(patsubst ../../common/%.c,$(BUILDDIR)/%.rel,$(COMMON))

Z80_CFLAGS := -mz80 --Werror --fsigned-char --std-sdcc99 --opt-code-speed --fomit-frame-pointer $(EXTRA_CFLAGS) -I../../common
Z80_LDFLAGS := --no-std-crt0
CODE := 0x0080
DATA := 0xE000

all: $(ROMSET)

run: $(ROMSET)
	$(MAME) -rp $(ROMPATH) $(PCB_VARIANT) -window

debug: $(ROMSET)
	$(MAME) -rp $(ROMPATH) $(PCB_VARIANT) -window -debug

# This is broken, so for now we need to export png manually whenever we change artwork:
#mines.png: mines.xcf
#	convert mines.xcf mines.png

mines.xpm: mines.png
	convert mines.png mines.xpm

#mines_background.xpm: mines_background.xcf
#	convert mines_background.xcf mines_background.xpm

tools/patch_CGROM: mines.xpm tools/patch_CGROM.c char_codes.h
	$(CC) tools/patch_CGROM.c -o tools/patch_CGROM

#tools/patch_BGROM: mines_background.xpm tools/patch_BGROM.c
#	$(CC) tools/patch_BGROM.c -o tools/patch_BGROM

gs01.11f: tools/patch_CGROM
	cp $(ROMPATH)/gunsmoke/gs01.11f .
	./tools/patch_CGROM
	cp gs01.11f $(ROMPATH)/gunsmoke/

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
#	cp $(ROMPATH)/gunsmoke/*.bin .
#	./tools/patch_BGROM
#	cp *.bin $(ROMPATH)/gunsmoke/

$(BUILDDIR)/maincpu.ihx: $(OBJECTS) $(C_OBJECTS) $(BUILDDIR)/boot.rel
	$(Z80COMPILER) $(Z80_CFLAGS) $(Z80_LDFLAGS) $(LIBS) --code-loc $(CODE) --data-loc $(DATA) $(BUILDDIR)/boot.rel $(OBJECTS) $(C_OBJECTS) -o $(BUILDDIR)/maincpu.ihx

$(BUILDDIR)/%.rel: ../../common/%.c $(BUILDDIR)
	$(Z80COMPILER) $(Z80_CFLAGS) $(Z80_LDFLAGS) -c $< -o $@

$(BUILDDIR)/%.rel: ./%.c $(BUILDDIR)
	$(Z80COMPILER) $(Z80_CFLAGS) $(Z80_LDFLAGS) -c $< -o $@

$(BUILDDIR)/boot.rel: boot.s
	$(Z80ASSEMBLER) -g -o $@ $<

3.ic85: $(BUILDDIR)/maincpu.ihx
	$(OBJCOPY) $(BUILDDIR)/maincpu.ihx -I ihex -O binary 3.ic85
	cp 3.ic85 $(ROMPATH)/gunsmoke/


gs03.09n: $(BUILDDIR)/maincpu.ihx
	$(OBJCOPY) $(BUILDDIR)/maincpu.ihx -I ihex -O binary gs03.09n
	cp gs03.09n $(ROMPATH)/gunsmoke/

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

clean:
	rm -r $(BUILDDIR)
	rm $(ROMSET)
	rm tools/patch_CGROM
#	rm tools/patch_BGROM
	rm mines*.xpm
