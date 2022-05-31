SOURCES=video.c input.c ../../common/main.c  ../../common/minefield.c
BUILDDIR=build/
CFLAGS=-I../../common

all: $(SOURCES)
	mkdir -p $(BUILDDIR)
	gcc $(SOURCES) $(CFLAGS) -lncurses -o $(BUILDDIR)mines

clean:
	rm -r $(BUILDDIR)
