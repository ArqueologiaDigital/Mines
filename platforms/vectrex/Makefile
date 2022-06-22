CC := m6809-unknown-none-gcc
SOURCES := crt0.s video.c input.c extras.c ../../common/minefield.c ../../common/video-tiles.c ../../common/main.c
BUILDDIR := build
CFLAGS := -std=c99 -Os -I../../common -I. -DNO_UNUSED_MACRO -DMINEFIELD_ALLOCATION
SCREEN_RESOLUTION := -DSCREEN_WIDTH=320/8 -DSCREEN_HEIGHT=200/8 -DMINEFIELD_X_OFFSET=10 -DMINEFIELD_Y_OFFSET=2
ROMPATH := ~/hdd/fsanches/nonfree/MAMEROMs/
MAME := ~/mame/mame

all: $(SOURCES)
	mkdir -p $(BUILDDIR)
	$(CC) -o mines.srec $(CFLAGS) $(SCREEN_RESOLUTION) $(SOURCES)
	objcopy --input-target=srec --output-target=binary mines.srec mines.vec
	rm mines.srec

run: all
	$(MAME) -rp $(ROMPATH) vectrex -cart mines.vec

debug: all
	$(MAME) -rp $(ROMPATH) vectrex -cart mines.vec -debug

clean:
	rm -rf $(BUILDDIR)
