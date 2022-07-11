CC := ia16-elf-gcc

# extras.c implements _start and must be the first one here.
SOURCES := extras.c audio.c video.c input.c ../../common/main.c ../../common/minefield.c ../../common/video-tiles.c
BUILDDIR := build
CFLAGS :=  -nodefaultlibs -nostdlib -mcmodel=tiny -mregparmcall -march=i186 -Os -Wall -I../../common -I. -DNO_UNUSED_MACRO
LDFLAGS :=
SCREEN_RESOLUTION := -DSCREEN_WIDTH=320/8 -DSCREEN_HEIGHT=200/8 -DMINEFIELD_X_OFFSET=10 -DMINEFIELD_Y_OFFSET=2

PYTHON := python3

all: $(SOURCES)
	mkdir -p $(BUILDDIR)
	$(PYTHON) ./compress-mines-xpm.py build/mines.bin
	$(CC) $(SOURCES) $(CFLAGS) $(LDFLAGS) $(SCREEN_RESOLUTION) -o $(BUILDDIR)/mines.com

run: all
	dosbox $(BUILDDIR)

clean:
	rm -rf $(BUILDDIR)
