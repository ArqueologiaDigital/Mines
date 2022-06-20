CC=gcc
SOURCES=extras.c video.c input.c ../../common/main.c ../../common/minefield.c ../../common/video-tiles.c
BUILDDIR=build
CFLAGS=-Wall -pedantic -I../../common -I. -DNO_UNUSED_MACRO -DMINEFIELD_ALLOCATION $(shell pkg-config --cflags gl glu glut)
LDFLAGS = $(shell pkg-config gl glu glut --libs) -lm
SCREEN_RESOLUTION := -DSCREEN_WIDTH=320/8 -DSCREEN_HEIGHT=200/8 -DMINEFIELD_X_OFFSET=10 -DMINEFIELD_Y_OFFSET=2 -DGLUT_BASED_IMPLEMENTATION

all: $(SOURCES)
	mkdir -p $(BUILDDIR)
	$(CC) $(SOURCES) $(CFLAGS) $(LDFLAGS) $(SCREEN_RESOLUTION) -o $(BUILDDIR)/mines

run: all
	$(BUILDDIR)/mines

clean:
	rm -rf $(BUILDDIR)
