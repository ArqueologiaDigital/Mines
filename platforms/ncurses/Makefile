SOURCES=video.c input.c ../../common/main.c  ../../common/minefield.c ../../common/video-tiles.c
BUILDDIR=build
SCREEN_RESOLUTION = -DMINEFIELD_X_OFFSET=1 -DMINEFIELD_Y_OFFSET=1
CFLAGS=-Wall -pedantic -I../../common -I. -DNO_UNUSED_MACRO -DDRAW_GAME_OVER -DDRAW_TITLE_SCREEN -DDRAW_COUNTER -DDRAW_TIMER

all: $(SOURCES)
	mkdir -p $(BUILDDIR)
	gcc $(SOURCES) $(CFLAGS) $(SCREEN_RESOLUTION) -lncurses -o $(BUILDDIR)/mines

run: all
	./build/mines

clean:
	rm -rf $(BUILDDIR)
