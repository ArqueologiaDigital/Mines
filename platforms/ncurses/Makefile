SOURCES=video.c input.c ../../common/main.c  ../../common/minefield.c
BUILDDIR=build
CFLAGS=-Wall -pedantic -I../../common -I. -DNO_UNUSED_MACRO -DDRAW_GAME_OVER -DDRAW_TITLE_SCREEN

all: $(SOURCES)
	mkdir -p $(BUILDDIR)
	gcc $(SOURCES) $(CFLAGS) -lncurses -o $(BUILDDIR)/mines

clean:
	rm -rf $(BUILDDIR)
